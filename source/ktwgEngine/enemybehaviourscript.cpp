#include "enemybehaviourscript.h"
#include "gameplaymanager.h"
#include "time.h"
#include "scene.h"

#if SERVER
#include "enemymanagerscript.h"
#endif

EnemyBehaviour::EnemyBehaviour(Entity & entity)
:Behaviour{typeid(EnemyBehaviour), entity}
,m_CurrentTime{0.0f}
,m_PollingInterval{10.0f}
,m_Speed{2.0f}
,m_Health{2}
{
}

EnemyBehaviour::~EnemyBehaviour()
{
}

void EnemyBehaviour::Start()
{
  const Vec3 currentPos = m_Entity->GetTransform().GetPosition();
  UpdateTarget(currentPos);

#if SERVER
  m_Manager = Scene::GetInstance().FindEntityByName("enemyMng")->GetComponent<EnemyManager>();
#endif
}

void EnemyBehaviour::Update()
{
  const float dt = (float)Time().GetDeltaTime();
  const Vec3 currentPos = m_Entity->GetTransform().GetPosition();

  if (m_CurrentTime >= m_PollingInterval)
  {
    UpdateTarget(currentPos);
    m_CurrentTime = 0.0f;
  }
  
  ChaseTarget(currentPos);

  m_CurrentTime += dt;
}

void EnemyBehaviour::TakeDamage(unsigned damage)
{
  m_Health -= damage;

  if (m_Health <= 0)
  {
    m_Entity->SetActive(false);

#if SERVER
    m_Manager->OnEnemyDeath();
#endif
  }
}

void EnemyBehaviour::Reset()
{
  m_Health = 2;
}

void EnemyBehaviour::ChaseTarget(const Vec3 & currentPos)
{
  if (m_Target)
  {
    const Vec3 targetPos = m_Target->GetTransform().GetPosition();

    Vec3 dir = targetPos - currentPos;
    const float sqDist = SqLength(dir);
    if (sqDist >= FLT_EPSILON)
    {
      dir /= sqrt(sqDist);
      const Quaternion rot = LookRotation(dir);
      m_Entity->GetTransform().SetRotation(rot);
      Vec3 vel = dir * m_Speed;
      vel.y_ = GetComponent<CRigidBody>()->GetLinearVelocity().y_;
      GetComponent<CRigidBody>()->SetLinearVelocity(vel);
      return;
    } 
  }

  GetComponent<CRigidBody>()->SetLinearVelocity(Vec3{});
}

void EnemyBehaviour::UpdateTarget(const Vec3& currentPos)
{
  m_Target = GameplayManager::GetInstance().GetNearestPlayer(currentPos);
}
