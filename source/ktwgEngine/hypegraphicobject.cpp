#include "hypegraphicobject.h"
#include "entity.h"
#include <algorithm>

HypeGraphicObjectInstance::HypeGraphicObjectInstance(Entity & owner, HypeGraphicObject* graphicObject)
:m_Owner{&owner}, m_GraphicObject{graphicObject}, m_Material{nullptr}
{
}

void HypeGraphicObjectInstance::CreateOverrideMaterial()
{
  m_Material = new HypeMaterial{};
}

void HypeGraphicObjectInstance::DestroyOverrideMaterial()
{
  delete m_Material;
  m_Material = nullptr;
}

HypeMaterial * HypeGraphicObjectInstance::GetMaterial() const
{
  if(m_Material)
    return m_Material;
  return m_GraphicObject->GetMaterial();
}

HypeGraphicObject::HypeGraphicObject()
:m_Instances{}, m_Name{}, m_Material{}
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
