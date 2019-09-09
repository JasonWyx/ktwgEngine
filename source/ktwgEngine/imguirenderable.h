#pragma once

class ImGuiRenderable
{
public:
  ImGuiRenderable();
  virtual ~ImGuiRenderable();

  virtual void RenderImGui() = 0;
};