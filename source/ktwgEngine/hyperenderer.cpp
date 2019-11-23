#include "hyperenderer.h"

#include "Shaders/cpp/ShaderCommon.h"
#include "d3d11shader.h"
#include "d3d11staticresources.h"
#include <fstream>

DEFINE_STATIC_TEXTURE(FinalColorOutput);
DEFINE_STATIC_TEXTURE(MainRenderDepthStencil);
DECLARE_VS(SimpleForwardVS);
DECLARE_PS(SimpleForwardPS);

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
  delete SimpleForwardVS;
  delete SimpleForwardPS;
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
