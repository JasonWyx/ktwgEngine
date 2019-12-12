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
};