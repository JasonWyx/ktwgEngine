#pragma once
#include "singleton.h"
#include "gamerenderable.h"
#include <vector>

class GameRenderer : public Singleton<GameRenderer>
{
public:

  GameRenderer();
  ~GameRenderer();

  void Update(float deltaTime);
  void AddRenderable(GameRenderable* renderable);
  void RemoveRenderable(GameRenderable* renderable);

private:

  // Inherited via Singleton
  virtual void InitializeInternal() override;
  virtual void ShutdownInternal() override;

  std::vector<GameRenderable*> m_GameRenderables;
};