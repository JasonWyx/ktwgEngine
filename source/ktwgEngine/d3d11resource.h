#pragma once

#include "d3d11defines.h"
#include <d3d11.h>
#include <unordered_map>

class D3D11Device;

enum RESOURCE_VIEW_TYPE
{
  SRV,
  DSV,
  RTV
};

struct ResourceViewKey
{
  RESOURCE_VIEW_TYPE m_Type;
  DXGI_FORMAT        m_Format;
  uint32_t           m_MostDetailedMip;
};

inline bool operator==(const ResourceViewKey& lhs, const ResourceViewKey& rhs)
{
  return lhs.m_Type == rhs.m_Type && lhs.m_Format == rhs.m_Format && lhs.m_MostDetailedMip == rhs.m_MostDetailedMip;
}

namespace std
{
  template<>
  struct hash<ResourceViewKey>
  {
    size_t operator()(const ResourceViewKey& val) const noexcept
    {
      size_t hash;
      hash_combine(hash, (size_t)val.m_Type);
      hash_combine(hash, (size_t)val.m_Format);
      hash_combine(hash, (size_t)val.m_MostDetailedMip);
      return hash;
    }
  };
}

class D3D11ResourceView
{
public:

  D3D11ResourceView(const ComPtr<ID3D11ShaderResourceView>& srv)
    : m_View{ srv }
  {
  }

  D3D11ResourceView(const ComPtr<ID3D11DepthStencilView>& dsv)
    : m_View{ dsv }
  {

  }

  D3D11ResourceView(const ComPtr<ID3D11RenderTargetView>& rtv)
    : m_View{ rtv }
  {

  }

  ~D3D11ResourceView()
  {
    
  }

  ID3D11ShaderResourceView* GetSrv() const { return static_cast<ID3D11ShaderResourceView*>(m_View.Get()); }
  ID3D11RenderTargetView* GetRtv() const { return static_cast<ID3D11RenderTargetView*>(m_View.Get()); }
  ID3D11DepthStencilView* GetDsv() const { return static_cast<ID3D11DepthStencilView*>(m_View.Get()); }
  

private:
  friend class D3D11Resource;

  void SetSrv(const ComPtr<ID3D11ShaderResourceView>& srv) { m_View = srv; }
  void SetRtv(const ComPtr<ID3D11RenderTargetView>& rtv) { m_View = rtv; }
  void SetDsv(const ComPtr<ID3D11DepthStencilView>& dsv) { m_View = dsv; }

  ComPtr<ID3D11View>  m_View;
};

class D3D11Resource
{
public:

  D3D11Resource(const ComPtr<ID3D11Resource>& resource, D3D11_RESOURCE_DIMENSION dimension);

  const D3D11ResourceView& GetView(const ResourceViewKey& key);

  const D3D11ResourceView& CreateSRV(const ResourceViewKey& key);
  const D3D11ResourceView& CreateRTV(const ResourceViewKey& key);
  const D3D11ResourceView& CreateDSV(const ResourceViewKey& key);

protected:
  ComPtr<ID3D11Resource>                                  m_Resource;
private:
  std::unordered_map<ResourceViewKey, D3D11ResourceView>  m_ResourceViews;
  D3D11_RESOURCE_DIMENSION                                m_Dimension;
};
