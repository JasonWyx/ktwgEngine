#pragma once

#include <stdint.h>
#include "entity.h"
#include "typeinfo.h"

// Components
#include "cbehaviour.h"
#include "crigidbody.h"
#include "cboxcollider.h"

class InputSystem;
class Time;

class Behaviour
{
public:
  Behaviour(const ::TypeInfo& info, Entity& entity);
  virtual ~Behaviour();

  virtual void Init() {};
  virtual void Start() {};
  virtual void Update() {};

  virtual void OnCollisionEnter() {};
  virtual void OnCollisionStay() {};
  virtual void OnCollisionExit() {};
  virtual void OnTriggerEnter() {};
  virtual void OnTriggerStay() {};
  virtual void OnTriggerExit() {};

  static InputSystem&  Input();
  static Time& Time();

  inline const ::TypeInfo& GetTypeInfo() const { return m_Info; }

  template <typename T>
  T* GetComponent();

  Entity* m_Entity;
  ::TypeInfo m_Info;
};

template<typename T>
inline T* Behaviour::GetComponent()
{
  T* comp = m_Entity->GetComponent<T>();

  if (comp)
    return comp;
  
  auto& info = typeid(T);
  const auto& behaviourComp = m_Entity->GetAllComponentsOfType<CBehaviour>();
  for (auto& behComp : behaviourComp)
  {
    if (behComp->GetInternal()->GetTypeInfo() == info)
      return reinterpret_cast<T*>(behComp->GetInternal());
  }

  return nullptr;
}
