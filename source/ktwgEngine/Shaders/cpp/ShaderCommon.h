#pragma once

#include "../../d3d11shader.h"
#include "../../matrix4.h"

struct GeometryConstantBufferInputs
{
  Matrix4 m_ModelViewProjection;
  Matrix4 m_WorldInverseTranspose;
};

#define GeometryConstantBufferSlot 10
