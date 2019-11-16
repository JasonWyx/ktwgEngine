#pragma once
#include "singleton.h"
#include "debugrenderable.h"
#include <vector>

class DebugRenderer : public Singleton<DebugRenderer>
{
public:

    DebugRenderer();
    ~DebugRenderer();

    void Update(float deltaTime);
    void InsertDebugRenderable(DebugRenderable* renderable);
    void RemoveDebugRenderable(DebugRenderable* renderable);

private:

    void InitializeInternal() override;
    void ShutdownInternal() override;

    std::vector<DebugRenderable*> m_DebugRenderables;

};