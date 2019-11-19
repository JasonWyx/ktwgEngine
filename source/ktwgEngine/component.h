#pragma once
#include <memory>
#include "componenttype.h"

class Entity;

class Component
{
  template <typename T>
  using SharedPtr = std::shared_ptr<T>;

  friend class Entity;
  
  enum State : char
  {
    ACTIVE   = 0,
    INACTIVE = 1,
    DEAD     = 2,
    MODIFIED = 1 << 3
  };

public:
  Component(const SharedPtr<Entity>& entity, uint32_t id);
  virtual ~Component();

  virtual void Initialize() {};
  virtual void Destroy()    {};

  template<typename T>
  T& Get() { return static_cast<T&>(*this); }

  inline uint32_t GetId() const { return m_Id; }

private:
  SharedPtr<Entity> m_Owner;
  ComponentType     m_Type;
  State             m_State;
  uint32_t          m_Id;
};