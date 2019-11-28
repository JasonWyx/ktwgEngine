#include "EnemyManagerScript.h"
#include "GameplayManager.h"
#include "enemypoolscript.h"
#include "EnemyBehaviour.h"
#include "entity.h"
#include "time.h"

EnemyManager::EnemyManager(Entity& entity)
  : Behaviour{ typeid(EnemyManager), entity},
    m_SpawnInterval{ 30.0f },
    m_DeathCount{ 0 },
    m_AccumulateTime{ 0.0f }
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
  EnemyPool::Initialize();
  Spawn(GameplayManager::GetInstance().GetConnectedPlayers().size());
}

void EnemyManager::Update()
{
  m_AccumulateTime += static_cast<float>(Time().GetDeltaTime());

  if (m_AccumulateTime > m_SpawnInterval)
    Spawn(m_DeathCount);
}

void EnemyManager::Spawn(unsigned size)
{
  float j = 1.0f;
  for (unsigned i = 0; i < size; ++i)
  {
    Entity* obj = EnemyPool::GetInstance().GetObject();
    CBehaviour& beh = obj->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    beh.Bind<EnemyBehaviour>();
    
    Transform& tf = obj->GetTransform();
    tf.SetPosition(Vec3{j, j, j});

    obj->SetActive(true);
  }
}
