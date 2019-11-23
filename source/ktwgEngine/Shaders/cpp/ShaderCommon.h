#pragma once

#include "../../d3d11shader.h"

struct GeometryConstantBuffer
{
  float m_ModelViewProjection[16];
};

#define GeometryConstantBufferSlot 10
