#include "behaviour.h"

Behaviour::Behaviour(const ::TypeInfo& info, Entity& entity)
  : m_Entity{ &entity },
    m_Info{ info }
{
}

Behaviour::~Behaviour()
{
}
