#pragma once

#include "d3d11shader.h"
#include "d3d11renderapi.h"
#include "d3d11device.h"

template <>
inline D3D11Shader* CreateShader<VS>(const SHADER_DESC& desc)
{
  ComPtr<ID3DBlob> microCode = CompileShader("vs_5_0", desc);
  if (!microCode)
    return nullptr;
  ComPtr<ID3D11VertexShader> vs;
  HRESULT hr =D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateVertexShader(microCode->GetBufferPointer(), microCode->GetBufferSize(), NULL, vs.GetAddressOf());
  if (FAILED(hr))
  {
    std::cerr << "Failed to create vertex shader!\n";
    return nullptr;
  }
  return new D3D11VertexShader{ vs, microCode };
}

template <>
inline D3D11Shader* CreateShader<PS>(const SHADER_DESC& desc)
{
  ComPtr<ID3DBlob> microCode = CompileShader("ps_5_0", desc);
  if (!microCode)
    return nullptr;
  ComPtr<ID3D11PixelShader> ps;
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreatePixelShader(microCode->GetBufferPointer(), microCode->GetBufferSize(), NULL, ps.GetAddressOf());
  if (FAILED(hr))
  {
    std::cerr << "Failed to create vertex shader!\n";
    return nullptr;
  }
  return new D3D11PixelShader{ps, microCode};
}
