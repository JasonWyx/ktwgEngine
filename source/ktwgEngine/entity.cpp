#include "entity.h"
#include "entitypool.h"

uint32_t Entity::s_EntityCount = 0;

Entity::Entity(size_t id, const std::string& name = "")
  : m_Id{id}, m_Name{name}, m_State{ACTIVE}
{
}

Entity::~Entity()
{
}

Entity::SharedPtr<Entity> Entity::CreateEntity(const std::string& name)
{
  Entity& entity = EntityPool::Instance()->Create(name);
  return std::make_shared<Entity>(entity);
}

Entity& Entity::GetEntity(uint32_t id)
{
  return EntityPool::Instance()->Get(id);
}

Entity::SharedPtr<Entity> Entity::AddChild()
{
  return nullptr;
}

Entity::SharedPtr<Component> Entity::AddComponent(ComponentType type)
{
#define CASE_CREATE_COMP(comp_type, T) case comp_type: comp = CreateComponent<T>(comp_type); 

  SharedPtr<Component> comp;
  switch (type)
  {
    //CASE_CREATE_COMP(NONE, TestComponent) break;
    CASE_CREATE_COMP(C_RIGIDBODY, CRigidBody) break;
    // ...
  default: break;
  }
  return comp;

#undef CASE_CREATE_COMP
}
