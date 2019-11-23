#pragma once

#include "component.h"

class HypeGraphicObject;
class HypeGraphicObjectInstance;

class CRenderable : public Component
{
public:
  CRenderable(Entity& entity, uint32_t id);

  HypeGraphicObject*  GetGraphicObject() const { return m_GraphicObject; }
  void                SetGraphicObject(HypeGraphicObject* graphicObject);

private:
  HypeGraphicObject* m_GraphicObject;
  HypeGraphicObjectInstance* m_Instance;
};
