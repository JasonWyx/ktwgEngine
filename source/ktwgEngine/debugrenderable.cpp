#include "debugrenderable.h"
#include "debugrenderer.h"

DebugRenderable::DebugRenderable()
{
    DebugRenderer::GetInstance().RegisterDebugRenderable(this);
}

DebugRenderable::~DebugRenderable()
{
    DebugRenderer::GetInstance().UnregisterDebugRenderable(this);
}
