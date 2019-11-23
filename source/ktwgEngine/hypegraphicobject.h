#pragma once

#include <vector>
#include <string>

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

  const std::string& GetName() const { return m_Name; }
  void SetName(const std::string& name) { m_Name = name; }
private:
  virtual HypeGraphicObjectInstance* NotifyInstanceCreatedInternal(Entity& instance) = 0;

protected:
  std::vector<HypeGraphicObjectInstance*> m_Instances;
  std::string m_Name;
};