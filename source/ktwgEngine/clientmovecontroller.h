#pragma once
#include "behaviour.h"

class ClientMoveController : public Behaviour
{
public:
  ClientMoveController(Entity& entity);
  ~ClientMoveController();

  void Init() override;
  void Start() override;
  void Update() override;

private:
  bool m_ShouldSend;
};