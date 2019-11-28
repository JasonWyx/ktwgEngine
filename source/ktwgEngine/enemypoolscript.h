#pragma once

#include "objectpoolscript.h"

class EnemyPool : public ObjectPool
{
public:
  EnemyPool(Entity& entity);
  ~EnemyPool();

  void Init() override;
  void Start() override;
  void Update() override;
};