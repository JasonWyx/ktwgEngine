#include "gamerenderable.h"
#include "singletons.h"

GameRenderable::GameRenderable()
{
  GameRenderer::GetInstance().InsertGameRenderable(this);
}

GameRenderable::~GameRenderable()
{
  GameRenderer::GetInstance().RemoveGameRenderable(this);
}
