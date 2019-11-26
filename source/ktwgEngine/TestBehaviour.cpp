#include "TestBehaviour.h"
#include <iostream>
#include "inputsystem.h"
#include "time.h"

TestBehaviour::TestBehaviour(Entity& entity)
  : Behaviour{ typeid(TestBehaviour), entity }
{
}

TestBehaviour::~TestBehaviour()
{
}

void TestBehaviour::Start()
{
  std::cout << "TestBehaviour::Initialize()" << std::endl;
}

void TestBehaviour::Update()
{
  auto& inputSys = Input();
  auto dt = Time().GetDeltaTime();

  Vec3 translate {};
  if (inputSys.OnKeyDown(KTWG_D))
  {
    translate.x_ += 100.0f * dt;
  }
  if (inputSys.OnKeyDown(KTWG_A))
  {
    translate.x_ -= 100.0f * dt;
  }
  if (inputSys.OnKeyDown(KTWG_W))
  {
    translate.z_ += 100.0f * dt;
  }
  if (inputSys.OnKeyDown(KTWG_S))
  {
    translate.z_ -= 100.0f * dt;
  }
  auto& tfm = m_Entity->GetTransform();
  tfm.SetPosition(tfm.GetPosition() + translate);
}
