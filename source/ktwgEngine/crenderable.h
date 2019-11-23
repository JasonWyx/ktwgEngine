#pragma once

#include "component.h"

class HypeGraphicObject;

class CRenderable : public Component
{
public:
  CRenderable(Entity& entity, uint32_t id);

  HypeGraphicObject*  GetGraphicObject() const { return m_GraphicObject; }
  void                SetGraphicObject(HypeGraphicObject* graphicObject) { m_GraphicObject = graphicObject; }

private:
  HypeGraphicObject* m_GraphicObject;
};
