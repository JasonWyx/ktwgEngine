#pragma once
#include <memory>
#include "componenttype.h"

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
  Component(Entity& entity, uint32_t id);
  virtual ~Component();

  virtual void Initialize() {};
  virtual void Destroy()    {};

  template<typename T>
  T& Get() { return static_cast<T&>(*this); }

  inline Entity*              GetOwner()  const { return m_Owner; }
  inline const ComponentType& GetType()   const { return m_Type; }
  inline uint32_t             GetId()     const { return m_Id; }
  inline bool                 GetActive() const { return m_State == ACTIVE; }

  inline void SetOwner(Entity* owner)     { m_Owner = owner; }
  inline void SetType(ComponentType type) { m_Type = type; }
  inline void SetId(uint32_t id)          { m_Id = id; }
  inline void SetActive(bool active)      { m_State = active ? ACTIVE : INACTIVE; }

private:
  Entity*       m_Owner;
  ComponentType m_Type;
  State         m_State;
  uint32_t      m_Id;
};