#pragma once

#include <d3d11.h>
#include "d3d11defines.h"

class D3D11Texture
{
public:
  D3D11Texture(const ComPtr<ID3D11Resource>& texture);
  
  const ComPtr<ID3D11Resource>&             GetTexture() const { return m_Texture; }

private:
  ComPtr<ID3D11Resource>            m_Texture;
};
