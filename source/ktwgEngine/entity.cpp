#include "entity.h"
#include "entitypool.h"

// Components' headers
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"
#include "crenderable.h"
#include "ccamera.h"

Entity::Entity(uint32_t id, const std::string& name)
  : m_Parent{ nullptr }, m_Children{}, m_Components{}, 
    m_Transform{}, m_State{ ACTIVE }, m_Id{ id }, m_Name{ name }, m_LayerId{ 0 }
{
}

Entity::~Entity()
{
}

Entity* Entity::CreateEntity(const std::string& name)
{
  Entity& entity = EntityPool::Instance()->Create(name);
  return &entity;
}

Entity& Entity::GetEntity(uint32_t id)
{
  return EntityPool::Instance()->Get(id);
}

Entity* Entity::AddChild()
{
  Entity* ent = CreateEntity();
  m_Children.emplace_back(ent);
  return m_Children.back();
}

void Entity::AddChild(Entity* entity)
{
  m_Children.emplace_back(entity);
}

// @ ADD COMPONENT
Component* Entity::AddComponent(ComponentType type)
{
#define CASE_CREATE_COMP(comp_type, T) case comp_type: comp = &CreateComponent<T>(); 

  Component* comp = nullptr;
  switch (type)
  {
    //CASE_CREATE_COMP(NONE, TestComponent) break;
    CASE_CREATE_COMP(CT_RIGIDBODY, CRigidBody) break;
    CASE_CREATE_COMP(CT_BOXCOLLIDER, CBoxCollider) break;
    CASE_CREATE_COMP(CT_BEHAVIOUR, CBehaviour) break;
    CASE_CREATE_COMP(CT_RENDERABLE, CRenderable) break;
    CASE_CREATE_COMP(CT_CAMERA, CCamera) break;
    // ...
  default: break;
  }
  return comp;

#undef CASE_CREATE_COMP
}

void Entity::RemoveComponent(Component* comp)
{
  Component* tmp = comp;
  m_Components.erase(std::remove(m_Components.begin(), m_Components.end(), tmp), m_Components.end());
  Component::FreeComponent(comp);
}
