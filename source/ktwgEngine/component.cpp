#include "component.h"
#include <tuple>
#include "componentmanager.h"

#include "crigidbody.h"
#include "cboxcollider.h"

template<typename T>
Component& handle_get(Component* comp) 
{ 
  return ComponentManager<T>::Get(comp);
}

template<typename T>
void handle_release(Component* comp) 
{ 
  ComponentManager<T>::Free(comp);
}

template<typename T>
void clear() 
{ 
  ComponentManager<T>::RefreshFreeList();
}

#define MAKETUPLE(T) { &handle_get<T>, &handle_release<T>, &clear<T> }

// @ADD_COMPONENT
// ** ORDER MUST CORRESPOND TO COMPONENT ENUM!! **
Component::FUNC_PAIR Component::ComponentTable[ComponentType::END]
{
  { 0,0,0 },  // NONE
  MAKETUPLE(CRigidBody),
  MAKETUPLE(CBoxCollider)
};
#undef MAKETUPLE

Component::Component(const ::TypeInfo& info, Entity& entity, uint32_t id)
  : m_Owner{ &entity }, 
    m_Type{ CT_NONE },
    m_Info{ info },
    m_State{ ACTIVE }, 
    m_Id{ id }
{
}

Component::~Component()
{
}

void Component::FreeComponent(Component* comp)
{
  std::get<RELEASE>(ComponentTable[comp->m_Type])(comp);
}

void Component::RefreshFreeList()
{
  for (auto& cmng : ComponentTable)
  {
    auto fn = std::get<CLEAR>(cmng);
    if (!fn) continue;
    fn();
  }
}
