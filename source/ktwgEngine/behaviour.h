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
struct Collision;

class Behaviour
{
public:
  Behaviour(const ::TypeInfo& info, Entity& entity);
  virtual ~Behaviour();

  virtual void Init() {};
  virtual void Start() {};
  virtual void Update() {};

  static InputSystem&  Input();
  static Time& Time();

  virtual void OnCollisionEnter(Collision&) {};
  virtual void OnCollisionStay(Collision&) {};
  virtual void OnCollisionExit(Collision&) {};
  virtual void OnTriggerEnter(Collision&) {};
  virtual void OnTriggerStay(Collision&) {};
  virtual void OnTriggerExit(Collision&) {};

  Transform& Transform();
  
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
