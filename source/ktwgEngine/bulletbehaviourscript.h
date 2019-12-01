#pragma once

#include "behaviour.h"

class Entity;

class BulletBehaviour : public Behaviour
{
public:
  BulletBehaviour(Entity& entity);
  ~BulletBehaviour();

  void Start() override;
  void Update() override;

  void OnTriggerEnter(Collision& collider) override;

  void SetAttack(unsigned attack);

private:
  const float m_Speed = 50.0f;

  unsigned m_Attack;
};