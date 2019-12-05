#pragma once

class D3D11Device;

class HypeRenderNode
{
public:
  virtual void Setup(D3D11Device& device) {}
  virtual void Render(D3D11Device& device) {}
};
