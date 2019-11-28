#pragma once

#include <memory>
#include <string>
#include "singleton.h"
class Entity;

class Scene : public Singleton <Scene>
{
public:
	Scene();
	~Scene();

  Entity* CreateEntity(const std::string& name = "GameObject");

private:
	virtual void InitializeInternal() override;
	virtual void ShutdownInternal() override;

  Entity* m_GameScene;
};