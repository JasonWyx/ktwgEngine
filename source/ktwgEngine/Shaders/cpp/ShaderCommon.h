#pragma once


struct GeometryConstantBuffer
{
  float m_ModelViewProjection[16];
};

#define GeometryConstantBufferSlot 10

class D3D11Shader;

static D3D11Shader* SimpleForwardVS = nullptr;
static D3D11Shader* SimpleForwardPS = nullptr;
