#include "d3d11shader.h"
#include <d3dcompiler.h>

D3D11Shader::D3D11Shader(const ComPtr<ID3DBlob>& compiledShader)
:m_CompiledShader{compiledShader}
{
}

ComPtr<ID3DBlob> CompileShader(const char * profile, const SHADER_DESC & desc)
{
  static const D3D_SHADER_MACRO defines[]
  {
    {"HLSL", "1"},
    {nullptr, nullptr}
  };

  uint32_t compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#if defined(ENABLEDEBUGLAYER)
  compileFlags |= D3DCOMPILE_DEBUG;
  compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  ComPtr<ID3DBlob> microCode = nullptr;
  ComPtr<ID3DBlob> messages = nullptr;

  HRESULT hr = D3DCompile(desc.m_Source.c_str(), desc.m_Source.size(), nullptr, defines, nullptr, desc.m_Entry.c_str(), profile, compileFlags, 0, &microCode, &messages);

  if (FAILED(hr))
  {
    std::string message = std::string{ (const char*)messages->GetBufferPointer(), messages->GetBufferSize() };
    std::cerr << message << "\n";
    return nullptr;
  }

  return microCode;
}

D3D11Shader* CreateShaderCommon(const char * profile, const SHADER_DESC & desc)
{
  ComPtr<ID3DBlob> microCode = CompileShader(profile, desc);
  if(!microCode)
    return nullptr;
  return new D3D11Shader{microCode};
}
