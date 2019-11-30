#include "playercontroller.h"
#include "time.h"
#include "inputsystem.h"
#include "bulletpool.h"

PlayerController::PlayerController(Entity& entity)
  : Behaviour{ typeid(PlayerController), entity }
{
  m_Directions[PD_FORWARD] = Vec3{ 0.0f, 0.0f, 1.0f };
  m_Directions[PD_BACKWARD] = Vec3{ 0.0f, 0.0f, -1.0f };
  m_Directions[PD_LEFT] = Vec3{ -1.0f, 0.0f, 0.0f };
  m_Directions[PD_RIGHT] = Vec3{ 1.0f, 0.0f, 0.0f };
}

PlayerController::~PlayerController()
{
}



void PlayerController::Init()
{
  BulletPool::Initialize();
}

void PlayerController::Start()
{
}

void PlayerController::Update()
{
  UpdateAction();
  UpdateMovement();
}

void PlayerController::UpdateAction()
{
  if (Input().OnKeyPress(KTWG_SPACE))
    Fire();
}

void PlayerController::UpdateMovement()
{
  float dt = static_cast<float>(Time().GetDeltaTime());
  Vec3 movement;
  Vec3 direction;
  Quaternion rotation;
  m_DirectionFlag = 0;

  if (Input().OnKeyDown(KTWG_UP))
    m_DirectionFlag |= DIR_UP;
  if (Input().OnKeyDown(KTWG_DOWN))
    m_DirectionFlag |= DIR_DOWN;
  if (Input().OnKeyDown(KTWG_LEFT))
    m_DirectionFlag |= DIR_LEFT;
  if (Input().OnKeyDown(KTWG_RIGHT))
    m_DirectionFlag |= DIR_RIGHT;

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

  GetComponent<CRigidBody>()->SetLinearVelocity(movement);
  GetTransform().SetRotation(rotation);
}

void PlayerController::Fire()
{
  Entity* bullet = BulletPool::GetInstance().GetBullet();
  
  bullet->GetTransform().SetPosition(GetTransform().GetPosition());
  bullet->GetTransform().SetRotation(LookRotation(GetTransform().Forward()));
  bullet->SetActive(true);
}
