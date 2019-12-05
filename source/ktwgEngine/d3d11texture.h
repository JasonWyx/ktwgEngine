#pragma once

#include <d3d11.h>
#include <utility>
#include <unordered_map>
#include "d3d11defines.h"
#include "d3d11resource.h"

class D3D11Texture : public D3D11Resource
{
  ID3D11Texture2D* GetTexture2D() const { return static_cast<ID3D11Texture2D*>(m_Resource.Get()); }

public:
  D3D11Texture(const ComPtr<ID3D11Resource>& texture);
};
