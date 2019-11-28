#pragma once

#include "singleton.h"
#include <vector>

class Entity;

class ObjectPool : public Singleton<ObjectPool>
{
public:
  ObjectPool();
  ~ObjectPool();

  void IncreasePool(unsigned size);
  void SetObject(Entity* obj);
  Entity* GetObject();

protected:
  Entity* m_Object;    // Ptr to object stored in the pool
  std::vector<Entity*> m_Pool;
};