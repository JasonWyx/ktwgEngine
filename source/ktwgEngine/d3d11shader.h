#pragma once

#include <d3d11.h>
#include <iostream>
#include "d3d11defines.h"

enum SHADER_TYPE
{
  VS,
  PS
};

struct SHADER_DESC
{
  std::string m_Source;
  std::string m_Entry;
  SHADER_TYPE m_Type;  
};

class D3D11Shader
{
public:
  D3D11Shader(const ComPtr<ID3DBlob>&  compiledShader);

  const ComPtr<ID3DBlob>& GetCompiledShader() const { return m_CompiledShader; }

private:
  ComPtr<ID3DBlob>          m_CompiledShader;
};

ComPtr<ID3DBlob> CompileShader(const char* profile, const SHADER_DESC& desc);
D3D11Shader* CreateShaderCommon(const char* profile, const SHADER_DESC& desc);

template <SHADER_TYPE st>
D3D11Shader* CreateShader(const SHADER_DESC&)
{
  std::cerr << "Invalid Shader Type\n";
  return nullptr;
}

template <>
D3D11Shader* CreateShader<VS>(const SHADER_DESC& desc)
{
  return CreateShaderCommon("vs_5_0", desc);
}

template <>
D3D11Shader* CreateShader<PS>(const SHADER_DESC& desc)
{
  return CreateShaderCommon("ps_5_0", desc);
}
