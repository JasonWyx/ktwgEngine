#include "d3d11texture.h"

D3D11Texture::D3D11Texture(const ComPtr<ID3D11Resource>& texture)
:D3D11Resource{texture, D3D11_RESOURCE_DIMENSION_TEXTURE2D}
{

}
