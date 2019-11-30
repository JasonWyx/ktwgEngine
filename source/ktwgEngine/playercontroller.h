#pragma once

#include "behaviour.h"

class PlayerController : public Behaviour
{
  enum PlayerDirection {
    PD_FORWARD,
    PD_RIGHT,
    PD_BACKWARD,
    PD_LEFT,
    PD_MAX
  };

  enum DFlag {
    DIR_UP = 1,
    DIR_RIGHT = 1 << 1,
    DIR_DOWN = 1 << 2,
    DIR_LEFT = 1 << 3,
  };

public:
  PlayerController(Entity& entity);
  ~PlayerController();

  void Init() override;
  void Start() override;
  void Update() override;

private:
  void UpdateAction();
  void UpdateMovement();

  void Fire();

  const float m_Speed = 20.0f;

  Vec3 m_Directions[PD_MAX];
  uint8_t m_DirectionFlag;
};