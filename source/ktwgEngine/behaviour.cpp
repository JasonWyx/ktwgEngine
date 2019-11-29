#include "behaviour.h"

#include "inputsystem.h"
#include "time.h"
#include "scene.h"

Behaviour::Behaviour(const ::TypeInfo& info, Entity& entity)
  : m_Entity{ &entity },
    m_Info{ info },
    m_Active{ true }
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

void Behaviour::Set(Behaviour* comp)
{
  m_Active = comp->GetActive();
}

void Behaviour::SetActive(bool active)
{
  m_Active = active;
}

Transform& Behaviour::GetTransform()
{
  return m_Entity->GetTransform();
}

bool Behaviour::GetActive() const
{
  return m_Active;
}
