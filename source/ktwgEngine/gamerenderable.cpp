#include "gamerenderable.h"
#include "singletons.h"

GameRenderable::GameRenderable()
{
  GameRenderer::GetInstance().RegisterGameRenderable(this);
}

GameRenderable::~GameRenderable()
{
  GameRenderer::GetInstance().UnregisterGameRenderable(this);
}
