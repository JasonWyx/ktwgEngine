#pragma once

#include "behaviour.h"

class ObjectPool : public Behaviour
{

public:
  ObjectPool(const TypeInfo& type, Entity& entity);
  ~ObjectPool();

  virtual void Init() override;
  virtual void Start() override;
  virtual void Update() override;

  void IncreasePool(unsigned size);
  Entity* GetObject();

protected:
  Entity* m_Object;    // Ptr to object stored in the pool
  std::vector<Entity*> m_Pool;
};