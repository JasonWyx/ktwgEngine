#pragma once

#include <d3d11.h>
#include <iostream>
#include "d3d11defines.h"
#include "d3d11renderapi.h"

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
  virtual ~D3D11Shader(){}

  const ComPtr<ID3DBlob>& GetCompiledShader() const { return m_CompiledShader; }

  static D3D11Shader* CreateShader(const SHADER_DESC& desc);

private:
  ComPtr<ID3DBlob>          m_CompiledShader;
};

class D3D11VertexShader : public D3D11Shader
{
public:
  D3D11VertexShader(const ComPtr<ID3D11VertexShader>& vs, const ComPtr<ID3DBlob>&  compiledShader)
  :D3D11Shader{compiledShader}, m_Shader{vs}
  {

  }
  
  const ComPtr<ID3D11VertexShader>& GetShader() const { return m_Shader; }

private:
  ComPtr<ID3D11VertexShader> m_Shader;
};

class D3D11PixelShader : public D3D11Shader
{
public:
  D3D11PixelShader(const ComPtr<ID3D11PixelShader>& ps, const ComPtr<ID3DBlob>&  compiledShader)
  :D3D11Shader{compiledShader}, m_Shader{ps}
  {

  }

  const ComPtr<ID3D11PixelShader>& GetShader() const { return m_Shader; }
private:
  ComPtr<ID3D11PixelShader> m_Shader;
};

ComPtr<ID3DBlob> CompileShader(const char* profile, const SHADER_DESC& desc);

template <SHADER_TYPE st>
D3D11Shader* CreateShader(const SHADER_DESC&);
template <>
inline D3D11Shader* CreateShader<VS>(const SHADER_DESC& desc);
template <>
inline D3D11Shader* CreateShader<PS>(const SHADER_DESC& desc);

struct GFX_STATIC_RESOURCE_WRAPPER_VertexShader
{
  GFX_STATIC_RESOURCE_WRAPPER_VertexShader()
    :m_Resource{ nullptr }
  {
  }
  ~GFX_STATIC_RESOURCE_WRAPPER_VertexShader()
  {
    delete m_Resource;
  }
  D3D11VertexShader* m_Resource;
};

struct GFX_STATIC_RESOURCE_WRAPPER_PixelShader
{
  GFX_STATIC_RESOURCE_WRAPPER_PixelShader()
    :m_Resource{ nullptr }
  {
  }
  ~GFX_STATIC_RESOURCE_WRAPPER_PixelShader()
  {
    delete m_Resource;
  }
  D3D11PixelShader* m_Resource;
};

#define DEFINE_SHADER(name, tag) \
static GFX_STATIC_RESOURCE_WRAPPER_##tag gfx_static_resource_wrapper_##name;\
D3D11##tag* name = nullptr

#define DECLARE_SHADER(name, tag) extern D3D11##tag* name
#define DEFINE_VS(name) DEFINE_SHADER(name, VertexShader)
#define DEFINE_PS(name) DEFINE_SHADER(name, PixelShader)
#define DECLARE_VS(name) DECLARE_SHADER(name, VertexShader)
#define DECLARE_PS(name) DECLARE_SHADER(name, PixelShader)

#define CREATE_VS(desc) (D3D11VertexShader*)D3D11Shader::CreateShader(desc)
#define CREATE_PS(desc) (D3D11PixelShader*)D3D11Shader::CreateShader(desc)

#include "d3d11shader.hpp"
