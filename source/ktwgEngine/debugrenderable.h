#pragma once

class DebugRenderable
{
public:

    DebugRenderable();
    ~DebugRenderable();

    virtual void RenderDebug() = 0;
};