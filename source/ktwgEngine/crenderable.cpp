#include "crenderable.h"
#include "hypegraphicobject.h"

CRenderable::CRenderable(Entity & entity, uint32_t id)
:Component{typeid(CRenderable), entity, id}, m_GraphicObject{nullptr}, m_Instance{nullptr}
{
}

void CRenderable::SetGraphicObject(HypeGraphicObject * graphicObject)
{
  if (m_GraphicObject)
  {
    m_GraphicObject->NotifyInstanceDestroyed(m_Instance);
  }
  if (graphicObject)
  {
    m_Instance = graphicObject->NotifyInstanceCreated(*GetOwner());
  }
  m_GraphicObject = graphicObject;
}