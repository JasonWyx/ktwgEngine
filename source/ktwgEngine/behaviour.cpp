#include "behaviour.h"

#include "inputsystem.h"
#include "time.h"
#include "scene.h"

Behaviour::Behaviour(const ::TypeInfo& info, Entity& entity)
  : m_Entity{ &entity },
    m_Info{ info }
{
}

Behaviour::~Behaviour()
{
}

InputSystem& Behaviour::Input()
{
  return InputSystem::GetInstance();
}

Time& Behaviour::Time()
{
  return Time::GetInstance();
}

Scene& Behaviour::Scene()
{
  return Scene::GetInstance();
}

Transform& Behaviour::GetTransform()
{
  return m_Entity->GetTransform();
}
