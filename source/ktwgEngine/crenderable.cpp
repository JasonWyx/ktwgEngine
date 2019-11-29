#include "crenderable.h"
#include "hypegraphicobject.h"
#include "hypegraphicsworld.h"
#include "hypematerial.h"

CRenderable::CRenderable(Entity & entity, uint32_t id)
:Component{typeid(CRenderable), entity, id}, m_GraphicObject{nullptr}, m_Instance{nullptr}
{
  SetType(CT_RENDERABLE);
}

void CRenderable::Set(Component* comp)
{
  Component::Set(comp);

  CRenderable* renderable = static_cast<CRenderable*>(comp);

  if (renderable->GetGraphicObject())
    SetGraphicObject(renderable->m_GraphicObject->GetName());

  if (renderable->m_Instance->HasOverrideMaterial())
  {
    GetGraphicObjectInstance()->CreateOverrideMaterial();
    float r, g, b, a;
    renderable->m_Instance->GetMaterial()->GetColor(r, g, b, a);
    GetGraphicObjectInstance()->GetMaterial()->SetColor(r, g, b, a);
  }
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

void CRenderable::SetGraphicObject(const std::string & name)
{
  SetGraphicObject(HypeGraphicsWorld::GetInstance().GetGraphicObject(name));
}
