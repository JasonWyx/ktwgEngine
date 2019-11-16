#pragma once

class GameRenderer;

class GameRenderable
{
public:

  GameRenderable();
  ~GameRenderable();

  virtual void RenderGame() = 0;
};