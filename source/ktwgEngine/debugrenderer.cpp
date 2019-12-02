#include "debugrenderer.h"
#include <algorithm>
#include <glm/glm.hpp>

DebugRenderer::DebugRenderer()
{
}

DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::Update(float deltaTime)
{
    for (DebugRenderable* renderable : m_DebugRenderables)
    {
        renderable->RenderDebug();
    }
}

void DebugRenderer::RegisterDebugRenderable(DebugRenderable* renderable)
{
    m_DebugRenderables.push_back(renderable);
}

void DebugRenderer::UnregisterDebugRenderable(DebugRenderable* renderable)
{
    m_DebugRenderables.erase(std::remove(m_DebugRenderables.begin(), m_DebugRenderables.end(), renderable));
}

void DebugRenderer::InitializeInternal()
{
    // Generate square
    std::vector<glm::vec2> squareVertices;
    squareVertices.push_back(glm::vec2{ -0.5f, -0.5f });
    squareVertices.push_back(glm::vec2{ -0.5f,  0.5f });
    squareVertices.push_back(glm::vec2{ 0.5f,  0.5f });
    squareVertices.push_back(glm::vec2{ 0.5f, -0.5f });

    // Generate circle 
    std::vector<glm::vec2> circleVertices;
    constexpr unsigned segments = 24;
    constexpr float fullCircleRad = glm::radians(360.f);
    for (int i = 0; i < segments; ++i)
    {
        glm::vec2 vertex{ 0.f };
        vertex.x = glm::acos(fullCircleRad * i);
        vertex.y = glm::asin(fullCircleRad * i);
        circleVertices.push_back(vertex);
    }
}

void DebugRenderer::ShutdownInternal()
{
}
