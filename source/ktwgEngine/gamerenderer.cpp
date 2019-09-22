#include "gamerenderer.h"
#include <algorithm>
#include <glm/glm.hpp>

GameRenderer::GameRenderer()
{
}

GameRenderer::~GameRenderer()
{
}

void GameRenderer::Update(float deltaTime)
{
  for (GameRenderable* renderable : m_GameRenderables)
  {
    renderable->GameRender(*this);
  }
}

void GameRenderer::AddRenderable(GameRenderable* renderable)
{
  m_GameRenderables.push_back(renderable);
}

void GameRenderer::RemoveRenderable(GameRenderable* renderable)
{
  m_GameRenderables.erase(std::remove(m_GameRenderables.begin(), m_GameRenderables.end(), renderable));
}

void GameRenderer::InitializeInternal()
{
  // Generate square
  std::vector<glm::vec2> squareVertices;
  squareVertices.push_back(glm::vec2{-0.5f, -0.5f});
  squareVertices.push_back(glm::vec2{-0.5f,  0.5f});
  squareVertices.push_back(glm::vec2{ 0.5f,  0.5f});
  squareVertices.push_back(glm::vec2{ 0.5f, -0.5f});

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

void GameRenderer::ShutdownInternal()
{
}
