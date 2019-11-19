#pragma once

#include <memory>
#include "singleton.h"

class Entity;

class Scene : public Singleton <Scene>
{
  template <typename T>
  using SharedPtr = std::shared_ptr<T>;

public:
	Scene();
	~Scene();

private:
	virtual void InitializeInternal() override;
	virtual void ShutdownInternal() override;

  SharedPtr<Entity> m_GameScene;
};