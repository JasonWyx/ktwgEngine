#include "d3d11inputlayout.h"

D3D11InputLayout::D3D11InputLayout(const ComPtr<ID3D11InputLayout>& inputLayout)
:m_InputLayout{inputLayout}
{
}
