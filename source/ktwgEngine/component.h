#pragma once
#include <memory>
#include "componenttype.h"
#include "typeinfo.h"

class Entity;

class Component
{
  friend class Entity;
  
  enum State : char
  {
    ACTIVE   = 0,
    INACTIVE = 1,
    DEAD     = 2,
    MODIFIED = 1 << 3
  };

public:
  enum FUNC_TYPE { GET = 0, RELEASE, CLEAR };
  typedef std::tuple<Component& (*)(Component*), void(*)(Component*), void(*)(void)> FUNC_PAIR;
  static FUNC_PAIR ComponentTable[ComponentType::END];

  Component(const ::TypeInfo& info, Entity& entity, uint32_t id);
  virtual ~Component();

  virtual void Initialize() {};
  virtual void Destroy()    {};
  virtual void Set(Component* comp);

  template<typename T>
  T& Get() { return static_cast<T&>(*this); }

  inline Entity*              GetOwner()    const { return m_Owner; }
  inline const ComponentType& GetType()     const { return m_Type; }
  inline const ::TypeInfo&    GetTypeInfo() const { return m_Info; }
  inline uint32_t             GetId()       const { return m_Id; }
  inline bool                 GetActive()   const { return m_State == ACTIVE; }
  inline State                GetState()    const { return m_State; }

  inline void SetOwner(Entity* owner)             { m_Owner = owner; }
  inline void SetType(ComponentType type)         { m_Type = type; }
  inline void SetTypeInfo(const ::TypeInfo& info) { m_Info = info; }
  inline void SetId(uint32_t id)                  { m_Id = id; }
  inline virtual void SetActive(bool active)      { m_State = active ? ACTIVE : INACTIVE; }

  static void FreeComponent(Component* comp);
  static void RefreshFreeList();

private:
  Entity*       m_Owner;
  ComponentType m_Type;
  ::TypeInfo    m_Info;
  State         m_State;
  uint32_t      m_Id;
};