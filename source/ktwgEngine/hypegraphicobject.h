#pragma once

#include <vector>

class Entity;

class HypeGraphicObjectInstance
{
public:
  HypeGraphicObjectInstance(Entity& owner);
  virtual ~HypeGraphicObjectInstance() {}

protected:
  Entity* m_Owner;
};

class HypeGraphicObject
{
public:
  virtual ~HypeGraphicObject() {}
  virtual void DrawInstances() = 0;

  HypeGraphicObjectInstance* NotifyInstanceCreated(Entity& instance);
  void NotifyInstanceDestroyed(HypeGraphicObjectInstance* instance);

private:
  virtual HypeGraphicObjectInstance* NotifyInstanceCreatedInternal(Entity& instance) = 0;

protected:
  std::vector<HypeGraphicObjectInstance*> m_Instances;
};
