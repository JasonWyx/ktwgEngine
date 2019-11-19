#include "component.h"

Component::Component(const SharedPtr<Entity>& entity, uint32_t id)
  : m_Owner{ entity }, m_Type{ CT_NONE }, m_State{ ACTIVE }, m_Id{ id }
{
}

Component::~Component()
{
}
