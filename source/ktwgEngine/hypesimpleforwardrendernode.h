#pragma once

#include "hyperendernode.h"

class HypeSimpleForwardRenderNode : public HypeRenderNode
{
public:
  void Setup(D3D11Device& device) override;
  void Render(D3D11Device& device) override;
};
