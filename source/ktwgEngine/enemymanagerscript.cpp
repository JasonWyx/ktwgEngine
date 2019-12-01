#include "enemymanagerscript.h"
#include "gameplaymanager.h"
#include "enemypool.h"
#include "enemybehaviourscript.h"
#include "entity.h"
#include "randommodule.h"
#include "inputsystem.h"
#include "time.h"
#include <iostream>

EnemyManager::EnemyManager(Entity& entity)
  : Behaviour{ typeid(EnemyManager), entity},
    m_SpawnInterval{ 30.0f },
    m_DeathCount{ 0 },
    m_AccumulateTime{ 0.0f }
{
  // Create spawn points around border of the world
  float stride = 17.0f;
  float s = -85.0f;
  for (unsigned i = 0; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ s, 1.0f, -85.0f });
  
  s = -85.0f;
  for (unsigned i = 0; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ s, 1.0f, 85.0f });
  
  s = -85.0f + stride;
  for (unsigned i = 2; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ -85.0f, 1.0f, s });

  s = -85.0f + stride;
  for (unsigned i = 2; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ 85.0f, 1.0f, s });
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
  EnemyPool::Initialize();
}

void EnemyManager::Update()
{
  if (Input().OnKeyPress(KTWG_0))
  {
    Spawn(1);
    std::cout << "Spawn" << std::endl;
  }


  m_AccumulateTime += static_cast<float>(Time().GetDeltaTime());

  if (m_AccumulateTime > m_SpawnInterval)
    Spawn(m_DeathCount * 2);
}

void EnemyManager::Spawn(unsigned size)
{
  for (unsigned i = 0; i < size; ++i)
  {
    Entity* obj = EnemyPool::GetInstance().GetEnemy();
    
    Transform& tf = obj->GetTransform();
    tf.SetPosition(m_SpawnPosition[Random::Range(0, static_cast<int>(m_SpawnPosition.size() - 1))]);

    obj->SetActive(true);
  }
}
