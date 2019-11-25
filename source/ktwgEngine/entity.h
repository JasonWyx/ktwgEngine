#pragma once

#include <vector>
#include <string>
#include <memory>

#include "componenttype.h"
#include "componentmanager.h"
#include "transform.h"

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
    NONE     = 1 << 6
  };

public:
  Entity(uint32_t id, const std::string& name="");
  
  Entity(Entity&& rhs)            = default;
  Entity& operator=(Entity&& rhs) = default;

  Entity(const Entity& rhs)            = default;
  Entity& operator=(const Entity& rhs) = default;
  
  ~Entity();

  template<typename T>
  T* GetComponent();

  template<typename T> 
  container_t<T*> GetAllComponentsOfType();

  static Entity* CreateEntity(const std::string& name="Object");
  static Entity& GetEntity(uint32_t id);

  Entity*    AddChild();
  void       AddChild(Entity* entity);
  Component* AddComponent(ComponentType type);
  void       RemoveComponent(Component* comp);

  inline const Transform&               GetTransform()      const { return m_Transform; }
  inline const container_t<Component*>& GetComponents()     const { return m_Components; }
  inline const std::string&             GetName()           const { return m_Name; }
  inline uint32_t                       GetCollisionLayer() const { return m_LayerId; }
  inline bool                           IsDead()            const { return m_State == DEAD; }

  inline Transform& GetTransform() { return m_Transform; }

  inline void SetName(const std::string& name) { m_Name = name; }

private:
  template<typename T> 
  Component& CreateComponent();

  Entity*                 m_Parent;
  container_t<Entity*>    m_Children;
  container_t<Component*> m_Components;

  Transform   m_Transform;
  State       m_State;
  std::string m_Name;
  uint32_t    m_Id;
  uint32_t    m_LayerId;
};

template <typename T>
inline Component& Entity::CreateComponent()
{
  Component* comp = ComponentManager<T>::Alloc(*this);
  comp->Initialize();
  m_Components.push_back(comp);
  return *comp;
}

template <typename T>
T* Entity::GetComponent()
{
  const type_info& info = typeid(T);

  for (Component*& comp : m_Components)
  {
    if (comp->GetTypeInfo() == info)
      return static_cast<T*>(comp);
  }
  return nullptr;
}

template<typename T>
inline Entity::container_t<T*> Entity::GetAllComponentsOfType()
{
  auto& info = typeid(T);
  container_t<T*> result;

  for (auto& elem : m_Components)
  {
    if (elem->GetTypeInfo() == info)
      result.emplace_back(static_cast<T*>(elem));
  }

  return result;
}
