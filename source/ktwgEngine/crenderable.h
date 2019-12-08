#pragma once

#include "component.h"
#include <string>

class HypeGraphicObject;
class HypeGraphicObjectInstance;

class CRenderable : public Component
{
public:
  CRenderable(Entity& entity, uint32_t id);

  void Set(Component* comp) override;

  HypeGraphicObject*  GetGraphicObject() const { return m_GraphicObject; }
  HypeGraphicObjectInstance*  GetGraphicObjectInstance() const { return m_Instance; }
  void                SetGraphicObject(HypeGraphicObject* graphicObject);
  void                SetGraphicObject(const std::string& name);

  void GhostPropertyReadStream(BitStream& stream) override;
  void GhostPropertyWriteStream(BitStream& stream) override;

  static void GhostPropertyReadStream(Entity& entity, BitStream& stream);
  static void GhostPropertyWriteStream(CRenderable* renderable, BitStream& stream);

private:
  HypeGraphicObject* m_GraphicObject;
  HypeGraphicObjectInstance* m_Instance;
};
