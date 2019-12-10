#include "crenderable.h"
#include "hypegraphicobject.h"
#include "hypegraphicsworld.h"
#include "hypematerial.h"
#include "entity.h"
#include "componentids.h"

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

void CRenderable::GhostPropertyReadStream(BitStream & stream)
{
  bool hasOverrideMaterial;
  stream >> hasOverrideMaterial;

  uint8_t nameLen;
  // Assume the name length is less than 256
  stream >> nameLen;
  // Can't write strings into stream so just write each character instead
  char buf[256]{};
  for (uint8_t i = 0; i < nameLen; ++i)
  {
    stream >> buf[i];
  }
  std::string name {buf};
  SetGraphicObject(name);

  if (hasOverrideMaterial)
  {
    if (!m_Instance->HasOverrideMaterial())
    {
      m_Instance->CreateOverrideMaterial();
    }
    float r, g, b, a;
    uint8_t tmpR, tmpG, tmpB, tmpA;
    stream >> tmpR >> tmpG >> tmpB >> tmpA;
    r = tmpR / 255.0f;
    g = tmpG / 255.0f;
    b = tmpB / 255.0f;
    a = tmpA / 255.0f;
    m_Instance->GetMaterial()->SetColor(r, g, b, a);
  }
}

void CRenderable::GhostPropertyWriteStream(BitStream & stream)
{
  // ALWAYS PREFIX WITH CLASSID FOR COMPONENT AND COMPONENTTYPE DON'T NEED TO READ THIS BACK
  stream << CI_Component;
  stream << GetType();

  bool hasOverrideMaterial = m_Instance->HasOverrideMaterial();
  stream << hasOverrideMaterial;

  const std::string& name = m_GraphicObject->GetName();
  uint8_t nameLen = (uint8_t)name.length();
  // Assume the name length is less than 256
  stream << nameLen;
  // Can't write strings into stream so just write each character instead
  for (uint8_t i = 0; i < nameLen; ++i)
  {
    stream << name[i];
  }

  if (hasOverrideMaterial)
  {
    const HypeMaterial* material = m_Instance->GetMaterial();
    float r, g, b, a;
    material->GetColor(r, g, b, a);
    stream << (uint8_t)(r * 255.0f)
      << (uint8_t)(g * 255.0f)
      << (uint8_t)(b * 255.0f)
      << (uint8_t)(a * 255.0f);
  }
}

void CRenderable::RegisterAsGhostProperty(GhostObject * ghostObject, NetAuthority netAuthority)
{
  GhostPropertyComponent<CRenderable>* prop = new GhostPropertyComponent<CRenderable>{this, netAuthority};
  ghostObject->RegisterPropertyCustom(prop);
}
