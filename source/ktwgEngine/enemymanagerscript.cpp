#include "enemymanagerscript.h"
#include "gameplaymanager.h"
#include "enemypool.h"
#include "enemybehaviourscript.h"
#include "entity.h"
#include "randommodule.h"
#include "inputsystem.h"
#include "time.h"
#include "scene.h"
#include "gamestatemanagerscript.h"

#if SERVER
#include "event.h"
#include "streammanager.h"
#endif

EnemyManager::EnemyManager(Entity& entity)
  : Behaviour{ typeid(EnemyManager), entity},
    m_SpawnInterval{ Random::Range(5.0f, 10.0f) },
    m_SpawnCount{ Random::Range(1, 5) },
    m_CurrTime{ 0.0f }
{
  // Create spawn points around border of the world
  float stride = 17.0f;
  float s = -85.0f;
  for (unsigned i = 0; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ s, 5.0f, -85.0f });
  
  s = -85.0f;
  for (unsigned i = 0; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ s, 5.0f, 85.0f });
  
  s = -85.0f + stride;
  for (unsigned i = 2; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ -85.0f, 5.0f, s });

  s = -85.0f + stride;
  for (unsigned i = 2; i < 11; ++i, s += stride)
    m_SpawnPosition.emplace_back(Vec3{ 85.0f, 5.0f, s });
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
  EnemyPool::Initialize();
}

void EnemyManager::Start()
{
  m_WaveSize = 10;
  m_EnemiesLeft = 0;

  m_GSManager = Scene::GetInstance().FindEntityByName("gameStateMng")->GetComponent<GameStateManager>();
}

void EnemyManager::Update()
{
#if SERVER
  if (Input().GetInstance().OnKeyPress(KTWG_P))
    m_GSManager->SetIsGameStarted(true);
#endif

  if (!m_GSManager->GetIsGameStarted())
    return;

  if (Input().OnKeyPress(KTWG_0))
    Spawn(1);

  if (m_WaveSize > 0 && m_CurrTime <= 0.0f)
  {
    int spawnSize = m_WaveSize - m_SpawnCount;
    
    spawnSize = spawnSize < 0 ? m_WaveSize : m_SpawnCount;
    Spawn(spawnSize);
    m_EnemiesLeft += spawnSize;

    m_WaveSize -= m_SpawnCount;
    m_SpawnInterval = spawnSize * Random::Range(1.0f, 2.0f);
    m_SpawnCount = Random::Range(2, 8);
    m_CurrTime = m_SpawnInterval;

    //std::cout << m_WaveSize << " " << m_EnemiesLeft << std::endl;
  }

  m_CurrTime -= static_cast<float>(Time::GetInstance().GetFixedDeltaTime());
}

void EnemyManager::OnEnemyDeath()
{
  m_EnemiesLeft -= 1;

  if (m_EnemiesLeft <= 0)
  {
#if SERVER
    // Send activate win
    GameOverEvent* evt = new GameOverEvent;
    evt->m_Win = true;
    StreamManager::GetInstance().GetEventManager().BroadcastEvent(evt, true);
#else
    return;
#endif
  }
}

void EnemyManager::Spawn(unsigned size)
{
  for (unsigned i = 0; i < size; ++i)
  {
    Entity* obj = EnemyPool::GetInstance().GetEnemy();
    
    Transform& tf = obj->GetTransform();
    tf.SetPosition(m_SpawnPosition[Random::Range(0, static_cast<int>(m_SpawnPosition.size() - 1))]);

    obj->GetComponent<EnemyBehaviour>()->Start();

    obj->SetActive(true);
  }
}
