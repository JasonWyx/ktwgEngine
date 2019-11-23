#pragma once

#include <string>
#include <unordered_map>
#include "singleton.h"

class Entity;
class Camera;
class HypeGraphicObject;

class HypeGraphicsWorld : public Singleton<HypeGraphicsWorld>
{
public:
  void InitializeInternal() override;
  void ShutdownInternal() override;

  Camera* NotifyViewCreated(Entity& entity);
  void NotifyViewDestroyed();

  const std::vector<HypeGraphicObject*>& GetGraphicObjects() const { return m_GraphicObjects; }

private:
  std::vector<HypeGraphicObject*> m_GraphicObjects;
  Camera* m_View;
};
