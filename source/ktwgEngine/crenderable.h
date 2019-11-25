#pragma once

#include "component.h"
#include <string>

class HypeGraphicObject;
class HypeGraphicObjectInstance;

class CRenderable : public Component
{
public:
  CRenderable(Entity& entity, uint32_t id);

  HypeGraphicObject*  GetGraphicObject() const { return m_GraphicObject; }
  void                SetGraphicObject(HypeGraphicObject* graphicObject);
  void                SetGraphicObject(const std::string& name);

private:
  HypeGraphicObject* m_GraphicObject;
  HypeGraphicObjectInstance* m_Instance;
};
