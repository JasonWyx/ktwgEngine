#pragma once

#include "behaviour.h"

class PlayerController : public Behaviour
{
public:
  PlayerController(Entity& entity);
  ~PlayerController();

  void Start() override;
  void Update() override;

private:
  void UpdateMovement();

  const float m_Speed = 20.0f;
};