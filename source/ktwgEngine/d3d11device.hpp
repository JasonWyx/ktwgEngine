#pragma once

#include "d3d11device.h"
#include "d3d11hardwarebuffer.h"
#include "d3d11defines.h"

template <>
inline void D3D11Context::AddConstantBuffer<VS>(D3D11HardwareBuffer* buffer)
{
  m_VSConstantBuffers.emplace_back(buffer->GetBuffer());
}

template <>
inline void D3D11Context::AddConstantBuffer<PS>(D3D11HardwareBuffer* buffer)
{
  m_PSConstantBuffers.emplace_back(buffer->GetBuffer());
}