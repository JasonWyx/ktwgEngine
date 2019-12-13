#pragma once

#include "behaviour.h"

class GameStateManager;

class EnemyManager : public Behaviour
{
public:
  EnemyManager(Entity& entity);
  ~EnemyManager();

  void Init() override;
  void Start() override;
  void Update() override;

private:
  void Spawn(unsigned size);

  int m_SpawnCount;
  float m_SpawnInterval;
  float m_CurrTime;

  int m_WaveSize;
  int m_EnemiesLeft;

  std::vector<Vec3> m_SpawnPosition;

  GameStateManager* m_GSManager;
};