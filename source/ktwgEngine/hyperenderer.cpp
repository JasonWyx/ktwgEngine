#include "hyperenderer.h"

#include "Shaders/cpp/ShaderCommon.h"
#include "d3d11shader.h"
#include <fstream>

void HypeRenderer::LoadSimpleForward()
{
  std::fstream f{ "Shaders/hlsl/SimpleForward.hlsl" };

  SHADER_DESC desc;
  desc.m_Entry = "Shade_Vertex";
  desc.m_Source = std::string{ std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{} };
  desc.m_Type = VS;
  SimpleForwardVS = D3D11Shader::CreateShader(desc);
  desc.m_Entry = "Shade_Pixel";
  desc.m_Type = PS;
  SimpleForwardPS = D3D11Shader::CreateShader(desc);
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
