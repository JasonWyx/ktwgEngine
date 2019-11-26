#include "TestBehaviour.h"
#include <iostream>

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
{/*
  std::cout << "x : " << Transform().GetPosition().x_ << " " <<
               "y : " << Transform().GetPosition().y_ << " " <<
               "z : " << Transform().GetPosition().z_ << std::endl;*/
}
