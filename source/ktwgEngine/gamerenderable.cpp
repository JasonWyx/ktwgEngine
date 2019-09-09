#include "gamerenderable.h"
#include "singletons.h"

GameRenderable::GameRenderable()
{
  GameRenderer::GetInstance().AddRenderable(this);
}

GameRenderable::~GameRenderable()
{
  GameRenderer::GetInstance().RemoveRenderable(this);
}
