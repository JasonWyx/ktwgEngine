#pragma once

#include "behaviour.h"

class EnemyBehaviour : public Behaviour
{
public:
  EnemyBehaviour(Entity& entity);
  ~EnemyBehaviour();

  void Start() override;
  void Update() override;

private:

  void ChaseTarget(const Vec3& currentPos);
  void UpdateTarget(const Vec3& currentPos);

  Entity* m_Target;
  float m_CurrentTime;
  float m_PollingInterval;
  float m_Speed;
};