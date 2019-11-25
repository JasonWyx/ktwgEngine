#include "hypegraphicobject.h"
#include "entity.h"
#include <algorithm>

HypeGraphicObjectInstance::HypeGraphicObjectInstance(Entity & owner)
:m_Owner{&owner}
{
}

HypeGraphicObjectInstance * HypeGraphicObject::NotifyInstanceCreated(Entity & instance)
{
  HypeGraphicObjectInstance* newInstance = NotifyInstanceCreatedInternal(instance);
  if(newInstance)
    m_Instances.emplace_back(newInstance);
  return newInstance;
}

void HypeGraphicObject::NotifyInstanceDestroyed(HypeGraphicObjectInstance * instance)
{
  m_Instances.erase(std::remove(m_Instances.begin(), m_Instances.end(), instance), m_Instances.end());
  delete instance;
}
