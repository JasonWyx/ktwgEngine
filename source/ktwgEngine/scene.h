#pragma once

#include <memory>
#include "singleton.h"

class Entity;

class Scene : public Singleton <Scene>
{
public:
	Scene();
	~Scene();

private:
	virtual void InitializeInternal() override;
	virtual void ShutdownInternal() override;

  Entity* m_GameScene;
};