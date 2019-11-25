#include "SimpleForwardParams.h"
#include "../../d3d11staticresources.h"
#include "../../d3d11device.h"
#include "../../d3d11renderapi.h"
#include "../../d3d11hardwarebuffer.h"

DEFINE_STATIC_BUFFER(SimpleForwardParamsCbuf);

void ShaderInputs::SimpleForwardParams::InitializeHWResources()
{
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  GET_STATIC_RESOURCE(SimpleForwardParamsCbuf) = new D3D11HardwareBuffer{ device, D3D11_BT_CONSTANT, D3D11_USAGE_DYNAMIC, 4, sizeof(float), false, false, true, false };
}

void ShaderInputs::SimpleForwardParams::SetColor(float r, float g, float b, float a)
{
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
  m_Color[3] = a;
}

void ShaderInputs::SimpleForwardParams::GetColor(float & r, float & g, float & b, float & a)
{
  r = m_Color[0];
  g = m_Color[1];
  b = m_Color[2];
  a = m_Color[3];
}

void ShaderInputs::SimpleForwardParams::Set()
{
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  device->GetImmediateContext().AddConstantBuffer<VS>(GET_STATIC_RESOURCE(SimpleForwardParamsCbuf));
  device->GetImmediateContext().FlushConstantBuffers(SimpleForwardParamsSlot);

  GET_STATIC_RESOURCE(SimpleForwardParamsCbuf)->Write(0, 4 * sizeof(float), m_Color, WT_DISCARD);
}