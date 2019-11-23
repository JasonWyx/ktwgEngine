#pragma once

#include <stdint.h>
#include "entity.h"
#include "typeinfo.h"

class Behaviour
{
public:
  Behaviour(const ::TypeInfo& info, Entity& entity);
  virtual ~Behaviour();

  virtual void Initialize() = 0;
  virtual void Update()     = 0;
  
  inline const ::TypeInfo& GetTypeInfo() const { return m_Info; }

  /*template <typename T>
  T* GetComponent();*/

  Entity* m_Entity;
  ::TypeInfo m_Info;
};

//template<typename T>
//inline T* Behaviour::GetComponent()
//{
//  T* comp = m_Entity->GetComponent<T>();
//
//  if (comp)
//    return comp;
//  
//  auto& info = typeid(T);
//  auto& behaviourComp = m_Entity->GetAllComponentsOfType<CBehaviour>();
//  for (auto& behComp : behaviourComp)
//  {
//    if (behComp->GetInternal()->GetTypeInfo() == info)
//      return static_cast<T*>(behComp->GetInternal);
//  }
//}
