#pragma once
#include "behaviour.h"

class GameStateManager;

class ClientMoveController : public Behaviour
{
public:
  ClientMoveController(Entity& entity);
  ~ClientMoveController();

  void Init() override;
  void Start() override;
  void Update() override;

private:
  GameStateManager* m_GSManager;

  bool m_ShouldSend;
  float m_FireRate = 1.5f;
  float m_CoolDown = 0.0;
};