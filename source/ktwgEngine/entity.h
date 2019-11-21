#pragma once

#include <vector>
#include <string>
#include <memory>

#include "componenttype.h"
#include "componentmanager.h"

class Component;

class Entity
{
  template <typename T>
  using container_t = std::vector<T>;

  enum State : char 
  { 
    ACTIVE   = 0, 
    INACTIVE = 1, 
    DEAD     = 1 << 2, 
    NONE     =  1 << 6
  };

  static uint32_t s_EntityCount;
public:
  Entity(uint32_t id, const std::string& name="");
  
  Entity(Entity&& rhs)            = default;
  Entity& operator=(Entity&& rhs) = default;

  Entity(const Entity& rhs)            = default;
  Entity& operator=(const Entity& rhs) = default;
  
  ~Entity();

  static Entity* CreateEntity(const std::string& name="Object");
  static Entity& GetEntity(uint32_t id);

  Entity&    AddChild();
  Component& AddComponent(ComponentType type);

  inline bool IsDead() const { return m_State == DEAD; }

private:
  template<typename T> 
  Component& CreateComponent();

  Entity*                 m_Parent;
  container_t<Entity*>    m_Children;
  container_t<Component*> m_Components;

  // Transform   m_Transform;
  State       m_State;
  std::string m_Name;
  uint32_t    m_Id;
};

template<typename T>
inline Component& Entity::CreateComponent()
{
  Component* comp = ComponentManager<T, INITIAL_COMP_SIZE>::Alloc(*this);
  comp->Initialize();
  m_Components.push_back(comp);
  return *comp;
}