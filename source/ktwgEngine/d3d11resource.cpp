#include "d3d11resource.h"
#include "d3d11renderapi.h"
#include "d3d11device.h"
#include <iostream>

D3D11Resource::D3D11Resource(const ComPtr<ID3D11Resource>& resource, D3D11_RESOURCE_DIMENSION dimension)
:m_Resource{resource}, m_Dimension{dimension}
{
}

const D3D11ResourceView & D3D11Resource::GetView(const ResourceViewKey & key)
{
  auto it = m_ResourceViews.find(key);
  if (it != m_ResourceViews.end())
  {
    return it->second;
  }
  // Otherwise create the view
  switch (key.m_Type)
  {
  case SRV:
    return CreateSRV(key);
  case DSV:
    return CreateDSV(key);
  case RTV:
    return CreateRTV(key);
  }
  throw;
}

const D3D11ResourceView & D3D11Resource::CreateSRV(const ResourceViewKey & key)
{
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
  switch (m_Dimension)
  {
  case D3D11_RESOURCE_DIMENSION_BUFFER:
    {
      ComPtr<ID3D11Buffer> res;
      m_Resource.As(&res);
      D3D11_BUFFER_DESC bufDesc;
      res->GetDesc(&bufDesc);
      
      srvDesc.Format = key.m_Format;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
      // TODO take in first element and element offset as parameters
      srvDesc.Buffer.FirstElement = 0;
      srvDesc.Buffer.ElementOffset = 0;
      srvDesc.Buffer.ElementWidth = bufDesc.StructureByteStride;
      srvDesc.Buffer.NumElements = bufDesc.ByteWidth / bufDesc.StructureByteStride;

      
    }
    break;
  case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
    {
      ComPtr<ID3D11Texture2D> res;
      m_Resource.As(&res);
      D3D11_TEXTURE2D_DESC texDesc;
      res->GetDesc(&texDesc);
      srvDesc.Format = key.m_Format;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
      srvDesc.Texture2D.MostDetailedMip = key.m_MostDetailedMip;
    }
    break;
  default:
    std::cerr << "Unsupported resource dimension\n";
    throw;
  }

  ComPtr<ID3D11ShaderResourceView> srv;
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateShaderResourceView(m_Resource.Get(), &srvDesc, srv.GetAddressOf());

  if (FAILED(hr))
  {
    std::cerr << "Failed to create SRV\n";
    throw;
  }

  auto val = m_ResourceViews.emplace(key, srv);
  return val.first->second;
}

const D3D11ResourceView & D3D11Resource::CreateRTV(const ResourceViewKey & key)
{
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
  ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

  switch (m_Dimension)
  {
  case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
    {
      ComPtr<ID3D11Texture2D> res;
      m_Resource.As(&res);
      D3D11_TEXTURE2D_DESC texDesc;
      res->GetDesc(&texDesc);
      rtvDesc.Format = key.m_Format;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      rtvDesc.Texture2D.MipSlice = 0;
    }
    break;
  default:
    throw;
  }

  ComPtr<ID3D11RenderTargetView> rtv;
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateRenderTargetView(m_Resource.Get(), &rtvDesc, rtv.GetAddressOf());

  if (FAILED(hr))
  {
    std::cerr << "Failed to create RTV\n";
    throw;
  }

  auto val = m_ResourceViews.emplace(key, rtv);
  return val.first->second;
}

const D3D11ResourceView & D3D11Resource::CreateDSV(const ResourceViewKey & key)
{
  D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
  ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

  switch (m_Dimension)
  {
  case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
    {
      ComPtr<ID3D11Texture2D> res;
      m_Resource.As(&res);
      D3D11_TEXTURE2D_DESC texDesc;
      res->GetDesc(&texDesc);
      dsvDesc.Format = key.m_Format;
      dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvDesc.Texture2D.MipSlice = 0;
    }
    break;
  default:
    throw;
  }

  ComPtr<ID3D11DepthStencilView> dsv;
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateDepthStencilView(m_Resource.Get(), &dsvDesc, dsv.GetAddressOf());

  if (FAILED(hr))
  {
    std::cerr << "Failed to create DSV\n";
    throw;
  }

  auto val = m_ResourceViews.emplace(key, dsv);
  return val.first->second;
}
