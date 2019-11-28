#include "EnemyBehaviour.h"
#include "GameplayManager.h"
#include "time.h"

EnemyBehaviour::EnemyBehaviour(Entity & entity)
:Behaviour{typeid(EnemyBehaviour), entity}
,m_CurrentTime{0.0f}
,m_PollingInterval{10.0f}
,m_Speed{5.0f}
{
}

EnemyBehaviour::~EnemyBehaviour()
{
}

void EnemyBehaviour::Start()
{
  const Vec3 currentPos = m_Entity->GetTransform().GetPosition();
  UpdateTarget(currentPos);
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

void EnemyBehaviour::ChaseTarget(const Vec3 & currentPos)
{
  if (m_Target)
  {
    const Vec3 targetPos = m_Target->GetTransform().GetPosition();

    Vec3 dir = targetPos - currentPos;
    const float sqDist = SqLength(dir);
    if (sqDist >= FLT_EPSILON)
    {
      dir.Normalize();
      const Quaternion rot = LookRotation(dir);
      m_Entity->GetTransform().SetRotation(dir);
      GetComponent<CRigidBody>()->SetLinearVelocity(dir * m_Speed);
      return;
    }
  }

  GetComponent<CRigidBody>()->SetLinearVelocity(Vec3{});
}

void EnemyBehaviour::UpdateTarget(const Vec3& currentPos)
{
  m_Target = GameplayManager::GetInstance().GetNearestPlayer(currentPos);
}
