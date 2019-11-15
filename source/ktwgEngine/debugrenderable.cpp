#include "debugrenderable.h"
#include "debugrenderer.h"

DebugRenderable::DebugRenderable()
{
    DebugRenderer::GetInstance().InsertDebugRenderable(this);
}

DebugRenderable::~DebugRenderable()
{
    DebugRenderer::GetInstance().RemoveDebugRenderable(this);
}
