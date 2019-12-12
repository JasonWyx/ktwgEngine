#pragma once

#include <vector>
#include <string>
#include <memory>

#include "componenttype.h"
#include "componentmanager.h"
#include "transform.h"
#include "cbehaviour.h"
#include "bitstream.h"
#include "ghostobject.h"

class Component;

class Entity
{
  template <typename T>
  using container_t = std::vector<T>;

public:
  enum State : char
  {
    ACTIVE = 0,
    INACTIVE = 1,
    DEAD = 1 << 2,
    NONE = 1 << 6
  };
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

  inline const container_t<Entity*>&    GetChildren()       const { return m_Children; }
  inline const Transform&               GetTransform()      const { return m_Transform; }
  inline const container_t<Component*>& GetComponents()     const { return m_Components; }
  inline const std::string&             GetName()           const { return m_Name; }
  inline uint32_t                       GetCollisionLayer() const { return m_LayerId; }
  inline bool                           GetActive()         const { return m_State == ACTIVE; }
  inline bool                           IsDead()            const { return m_State == DEAD; }

  inline Transform& GetTransform() { return m_Transform; }

  inline void SetName(const std::string& name) { m_Name = name; }

  inline bool GetIsGhost() const { return m_IsGhost; }
  inline GhostObject* GetGhostObject() const { return m_GhostObject; }

  void SetActive(bool active);
  void SetAllComponentsActive(bool active);
  void SetAllChildrenActive(bool active);
  void Set(Entity* ent);

  void MarkEntityForGhost();

  void MarkEntityAsGhost(GhostID ghostId);

  void RegisterEntityGhostProperties(NetAuthority netAuthority);

  // These functions are only for creation of ghost objects, following which 
  void ReplicateGhostObjectFromBitstream(BitStream& bitstream);

#if CLIENT
  void ReplicateGhostObjectToBitstream(BitStream& bitstream);
#else
  void ReplicateGhostObjectToBitstream(const PeerID targetPeerID, BitStream & bitstream);
#endif

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

  GhostObject* m_GhostObject; // Corresponding ghost object on the ghost manager
  bool         m_IsGhost; // If this is set then this entity is a ghost (Owned by another client)
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
      return reinterpret_cast<T*>(comp);
  }

  const auto& behaviours = GetAllComponentsOfType<CBehaviour>();

  for (auto& beh : behaviours)
  {
    if (beh->IsComponent(info))
      return reinterpret_cast<T*>(beh->GetInternal());
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

inline BitStream& operator<<(BitStream& stream, const Entity& entity)
{
  stream << CI_Entity;
  stream << entity.GetActive();
  return stream;
}

inline BitStream& operator>>(BitStream& stream, Entity& entity)
{
  ComponentID dummy;
  stream >> dummy;
  bool active;
  stream >> active;
  entity.SetActive(active);
  return stream;
}

inline BitStream& ReplicateFromStream(BitStream& stream, Entity& entity)
{
  bool active;
  stream >> active;
  entity.SetActive(active);

  return stream;

}

template<>
class CustomGhostProperty<Entity> : public GhostProperty
{
public:

  CustomGhostProperty(Entity& property, NetAuthority authority)
    : GhostProperty(authority)
    , m_Entity(property)
    , m_PrevActive(property.GetActive())
  { }

  virtual void WriteStream(BitStream& stream) override
  {
    stream << m_Entity;
  }

  virtual void ReadStream(BitStream& stream) override
  {
    stream >> m_Entity;
  }

  virtual bool IsPropertyChanged() const override
  {
    return m_PrevActive != m_Entity.GetActive();
  }

  virtual void Sync() override
  {
    m_PrevActive = m_Entity.GetActive();
  }

private:
  Entity& m_Entity;
  bool m_PrevActive;
};
