#pragma once

#include <d3d11.h>
#include "d3d11defines.h"

class D3D11Texture
{
public:
  D3D11Texture(ID3D11Resource* texture, ID3D11ShaderResourceView* srv, ID3D11DepthStencilView* dsv, ID3D11UnorderedAccessView* uav, ID3D11RenderTargetView* rtv);
  
  const ComPtr<ID3D11Resource>&             GetTexture() const { return m_Texture; }
  const ComPtr<ID3D11ShaderResourceView>&   GetSrv() const { return m_Srv; }
  const ComPtr<ID3D11DepthStencilView>&     GetDsv() const { return m_Dsv; }
  const ComPtr<ID3D11UnorderedAccessView>&  GetUav() const { return m_Uav; }
  const ComPtr<ID3D11RenderTargetView>&     GetRtv() const { return m_Rtv; }

private:
  ComPtr<ID3D11Resource>            m_Texture;
  ComPtr<ID3D11ShaderResourceView>  m_Srv;
  ComPtr<ID3D11DepthStencilView>    m_Dsv;
  ComPtr<ID3D11UnorderedAccessView> m_Uav;
  ComPtr<ID3D11RenderTargetView>    m_Rtv;
};
