#pragma once

#include "singleton.h"
#include <vector>

class Entity;

class EnemyPool : public Singleton<EnemyPool>
{
public:
  EnemyPool();
  ~EnemyPool();

  virtual void InitializeInternal() override {};
  virtual void ShutdownInternal() override {};

  Entity* GetEnemy();

private:
  void IncreasePool(unsigned size);

	Entity* m_Object;    // Ptr to object stored in the pool
	std::vector<Entity*> m_Pool;
};