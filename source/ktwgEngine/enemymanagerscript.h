#pragma once

#include "behaviour.h"

class EnemyManager : public Behaviour
{
public:
  EnemyManager(Entity& entity);
  ~EnemyManager();

  void Init() override;
  void Update() override;

private:
  void Spawn(unsigned size);

  unsigned m_DeathCount;
  float m_SpawnInterval;
  float m_AccumulateTime;

  std::vector<Vec3> m_SpawnPosition;
};