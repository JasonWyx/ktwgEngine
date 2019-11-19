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

  template <typename T>
  using SharedPtr = std::shared_ptr<T>;

  enum State : char 
  { 
    ACTIVE = 0, 
    INACTIVE = 1, 
    DEAD = 2, 
    NONE =  1 << 7
  };

  static uint32_t s_EntityCount;
public:
  Entity(uint32_t id, const std::string& name="");
  
  Entity(Entity&& rhs)            = default;
  Entity& operator=(Entity&& rhs) = default;

  Entity(const Entity& rhs)            = default;
  Entity& operator=(const Entity& rhs) = default;
  
  ~Entity();

  static SharedPtr<Entity> CreateEntity(const std::string& name="Object");
  static Entity&           GetEntity(uint32_t id);

  SharedPtr<Entity>&   AddChild();
  SharedPtr<Component> AddComponent(ComponentType type);

  inline bool IsDead() const { return m_State == DEAD; }

private:
  template<typename T> 
  SharedPtr<Component> CreateComponent();

  SharedPtr<Entity>                 m_Parent;
  container_t<SharedPtr<Entity>>    m_Children;
  container_t<SharedPtr<Component>> m_Components;

  // Transform   m_Transform;
  State       m_State;
  std::string m_Name;
  uint32_t    m_Id;
};

template<typename T>
inline Entity::SharedPtr<Component> Entity::CreateComponent()
{
  SharedPtr<Component> comp = ComponentManager<T, INITIAL_COMP_SIZE>::Alloc(std::make_shared<Entity>(*this));
  comp->Initialize();
  m_Components.push_back(comp);
  return nullptr;
}