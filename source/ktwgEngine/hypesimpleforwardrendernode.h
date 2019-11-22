#pragma once

#include "hyperendernode.h"

class HypeSimpleForwardRenderNode : public HypeRenderNode
{
public:
  void Setup() override;
  void Render() override;
};
