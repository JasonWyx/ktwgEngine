#pragma once

#include "behaviour.h"

class Entity;

class CameraBehaviour : public Behaviour
{
public:
  CameraBehaviour(Entity& entity);
  ~CameraBehaviour();

  void Start() override;
  void Update() override;

private:
  Entity* m_Target;
};