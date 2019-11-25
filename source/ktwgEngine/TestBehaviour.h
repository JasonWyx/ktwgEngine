#pragma once

#include "behaviour.h"

class TestBehaviour : public Behaviour
{
public:
  TestBehaviour(Entity& entity);
  ~TestBehaviour();

  void Start() override;
  void Update() override;
};