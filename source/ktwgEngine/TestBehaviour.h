#pragma once

#include "behaviour.h"

class TestBehaviour : public Behaviour
{
public:
  TestBehaviour(Entity& entity);
  ~TestBehaviour();

  void Initialize() override;
  void Update() override;
};