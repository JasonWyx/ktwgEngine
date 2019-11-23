#include "hyperenderer.h"

#include "Shaders/cpp/ShaderCommon.h"
#include "d3d11shader.h"
#include "d3d11staticresources.h"
#include "matrix4.h"
#include <fstream>

DEFINE_STATIC_TEXTURE(FinalColorOutput);
DEFINE_STATIC_TEXTURE(MainRenderDepthStencil);
DECLARE_VS(SimpleForwardVS);
DECLARE_PS(SimpleForwardPS);
DEFINE_STATIC_BUFFER(GeometryConstantBuffer);

void HypeRenderer::LoadSimpleForward()
{
  std::fstream f{ "Shaders/hlsl/SimpleForward.hlsl" };

  SHADER_DESC desc;
  desc.m_Entry = "Shade_Vertex";
  desc.m_Source = std::string{ std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{} };
  desc.m_Type = VS;
  SimpleForwardVS = CREATE_VS(desc);
  desc.m_Entry = "Shade_Pixel";
  desc.m_Type = PS;
  SimpleForwardPS = CREATE_PS(desc);
}

void HypeRenderer::UnloadSimpleForward()
{
}

void HypeRenderer::InitializeInternal()
{
}

void HypeRenderer::ShutdownInternal()
{
}

void HypeRenderer::LoadSystemShaders()
{
  LoadSimpleForward();
}

void HypeRenderer::UnloadSystemShaders()
{
  UnloadSimpleForward();
}

void HypeRenderer::CreateCommonResources()
{
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  GET_STATIC_RESOURCE(GeometryConstantBuffer) = new D3D11HardwareBuffer{device, D3D11_BT_CONSTANT, D3D11_USAGE_DYNAMIC, 1, sizeof(Matrix4), false, false, true, false};
  device->GetImmediateContext().AddConstantBuffer<VS>(GET_STATIC_RESOURCE(GeometryConstantBuffer));
  device->GetImmediateContext().AddConstantBuffer<PS>(GET_STATIC_RESOURCE(GeometryConstantBuffer));
}
