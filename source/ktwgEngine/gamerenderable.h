#pragma once

class GameRenderer;

class GameRenderable
{
public:

  GameRenderable();
  ~GameRenderable();

  virtual void GameRender(GameRenderer& renderer) = 0;
};