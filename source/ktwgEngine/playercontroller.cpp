#include "playercontroller.h"
#include "time.h"
#include "bulletpool.h"
#include "bulletbehaviourscript.h"
#include "event.h"
#include "collision.h"
#include "gameplaymanager.h"

#if SERVER
#include "streammanager.h"
#else
#include "inputsystem.h"
#endif

PlayerController::PlayerController(Entity& entity)
  : Behaviour{ typeid(PlayerController), entity }
{
  m_Directions[PD_FORWARD] = Vec3{ 0.0f, 0.0f, 100.0f };
  m_Directions[PD_BACKWARD] = Vec3{ 0.0f, 0.0f, -100.0f };
  m_Directions[PD_LEFT] = Vec3{ -100.0f, 0.0f, 0.0f };
  m_Directions[PD_RIGHT] = Vec3{ 100.0f, 0.0f, 0.0f };

}

PlayerController::~PlayerController()
{
#if SERVER
  StreamManager::GetInstance().GetMoveManager().UnregisterMoveObject(m_PeerID);
  BulletFireEvent::RemoveSubscriber(m_PeerID, this);
#endif
}

void PlayerController::Init()
{
  BulletPool::Initialize();
}

void PlayerController::Start()
{
  m_IsAlive = true;
  m_Rb = GetComponent<CRigidBody>();
#if SERVER
  BulletFireEvent::RegisterSubscriber(m_PeerID, this);
#endif
}

void PlayerController::Update()
{
  if (!m_IsAlive)
    return;

  if (!m_Rb)
    m_Rb = GetComponent<CRigidBody>();

  UpdateAction();
  UpdateMovement();
}

void PlayerController::OnCollisionEnter(Collision& other)
{
  if (other.entity->GetName() == "Enemy")
  {
    m_IsAlive = false;
    GetComponent<CRenderable>()->GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    GameplayManager::GetInstance().OnPlayerDeath();
  }
}

#if SERVER
void PlayerController::CreateMoveControlObject()
{
  StreamManager::GetInstance().GetMoveManager().RegisterMoveObject(m_PeerID, &m_ControlObject);
}
#endif

#if SERVER
void PlayerController::OnBulletFireEvent(BulletFireEvent * bulletFireEvent)
{
  Fire();
}
bool PlayerController::GetIsAlive() const
{
  return m_IsAlive;
}
#endif

void PlayerController::UpdateAction()
{
#if CLIENT
  if (Input().OnKeyPress(KTWG_SPACE))
    Fire();
#endif
}

void PlayerController::UpdateMovement()
{
  float dt = static_cast<float>(Time().GetDeltaTime());
  Vec3 movement;
  Vec3 direction;
  Quaternion rotation;
  m_DirectionFlag = 0;

  movement.y_ = m_Rb->GetLinearVelocity().y_;

#if CLIENT
  if (Input().OnKeyDown(KTWG_UP))
    m_DirectionFlag |= DIR_UP;
  if (Input().OnKeyDown(KTWG_DOWN))
    m_DirectionFlag |= DIR_DOWN;
  if (Input().OnKeyDown(KTWG_LEFT))
    m_DirectionFlag |= DIR_LEFT;
  if (Input().OnKeyDown(KTWG_RIGHT))
    m_DirectionFlag |= DIR_RIGHT;
#else
  if(m_ControlObject.m_MoveState[MoveStateFlags::Up])
    m_DirectionFlag |= DIR_UP;
  if (m_ControlObject.m_MoveState[MoveStateFlags::Down])
    m_DirectionFlag |= DIR_DOWN;
  if (m_ControlObject.m_MoveState[MoveStateFlags::Left])
    m_DirectionFlag |= DIR_LEFT;
  if (m_ControlObject.m_MoveState[MoveStateFlags::Right])
    m_DirectionFlag |= DIR_RIGHT;
#endif

  if (m_DirectionFlag == 0)
    rotation = GetTransform().GetRotation();
  else
  {
    direction += m_DirectionFlag & DIR_UP ? m_Directions[PD_FORWARD] : Vec3{};
    direction += m_DirectionFlag & DIR_DOWN ? m_Directions[PD_BACKWARD] : Vec3{};
    direction += m_DirectionFlag & DIR_LEFT ? m_Directions[PD_LEFT] : Vec3{};
    direction += m_DirectionFlag & DIR_RIGHT ? m_Directions[PD_RIGHT] : Vec3{};

    direction.Normalize();
    movement += m_Speed * direction;
    rotation = LookRotation(direction);
  }

  m_Rb->SetLinearVelocity(movement);
  GetTransform().SetRotation(rotation);
}

void PlayerController::Fire()
{
  Entity* bullet = BulletPool::GetInstance().GetBullet();
  
  bullet->GetTransform().SetPosition(GetTransform().GetPosition());
  bullet->GetTransform().SetRotation(LookRotation(GetTransform().Forward()));

  BulletBehaviour* bb = bullet->GetComponent<BulletBehaviour>();
  bb->SetAttack(m_Attack);

  bullet->SetActive(true);
}
