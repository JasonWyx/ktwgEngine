#pragma once

#include "singleton.h"
#include <vector>

class Entity;

class BulletPool : public Singleton<BulletPool>
{
public:
  BulletPool();
  ~BulletPool();

  virtual void InitializeInternal() override {};
  virtual void ShutdownInternal() override {};

  Entity* GetBullet();
  void OnPlayerConnected();

private:
  void IncreasePool(unsigned size);

  Entity* m_Object;    // Ptr to object stored in the pool
  std::vector<Entity*> m_Pool;
};