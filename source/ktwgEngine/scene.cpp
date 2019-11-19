#include "scene.h"
#include "entity.h"
#include <iostream>

Scene::Scene()
  : m_GameScene{ nullptr }
{
}

Scene::~Scene()
{
}

void Scene::InitializeInternal()
{
  m_GameScene = Entity::CreateEntity("Scene");
}

void Scene::ShutdownInternal()
{
}
