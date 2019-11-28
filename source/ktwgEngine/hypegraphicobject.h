#pragma once

#include <vector>
#include <string>
#include "hypematerial.h"

class Entity;
class HypeGraphicObject;

class HypeGraphicObjectInstance
{
public:
  HypeGraphicObjectInstance(Entity& owner, HypeGraphicObject* graphicObject);
  virtual ~HypeGraphicObjectInstance() { delete m_Material; }

  void CreateOverrideMaterial();
  void DestroyOverrideMaterial();

  HypeMaterial* GetMaterial() const;
  // This should only be used to set an overriding material, THE INSTANCE WILL OWN THE MATERIAL
  void SetMaterial(HypeMaterial* material) { m_Material = material; }

  bool HasOverrideMaterial() const { return m_Material != nullptr; }

protected:
  Entity* m_Owner;
  HypeGraphicObject* m_GraphicObject;
  HypeMaterial* m_Material; // Overriding material
};

class HypeGraphicObject
{
public:
  HypeGraphicObject();
  virtual ~HypeGraphicObject() {}
  virtual void DrawInstances() = 0;

  HypeGraphicObjectInstance* NotifyInstanceCreated(Entity& instance);
  void NotifyInstanceDestroyed(HypeGraphicObjectInstance* instance);

  const std::string& GetName() const { return m_Name; }
  void SetName(const std::string& name) { m_Name = name; }

  HypeMaterial* GetMaterial() { return &m_Material; }
  const HypeMaterial* GetMaterial() const { return &m_Material; }
    
private:
  virtual HypeGraphicObjectInstance* NotifyInstanceCreatedInternal(Entity& instance) = 0;

protected:
  std::vector<HypeGraphicObjectInstance*> m_Instances;
  std::string m_Name;
  HypeMaterial m_Material;
};
