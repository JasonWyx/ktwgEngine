#include "d3d11resource.h"
#include "d3d11renderapi.h"
#include "d3d11device.h"
#include <iostream>

void D3D11SR::Init(const ComPtr<ID3D11Resource>& resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc)
{
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateShaderResourceView(resource.Get(), &desc, m_Resource.GetAddressOf());
  if (FAILED(hr))
  {
    std::cerr << "Failed to create shader resource view!\n";
  }
}

void D3D11DS::Init(const ComPtr<ID3D11Resource>& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC & desc)
{
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateDepthStencilView(resource.Get(), &desc, m_Resource.GetAddressOf());
  if (FAILED(hr))
  {
    std::cerr << "Failed to create depth stencil view!\n";
  }
}
