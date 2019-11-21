#include "component.h"

Component::Component(Entity& entity, uint32_t id)
  : m_Owner{ &entity }, m_Type{ CT_NONE }, m_State{ ACTIVE }, m_Id{ id }
{
}

Component::~Component()
{
}
