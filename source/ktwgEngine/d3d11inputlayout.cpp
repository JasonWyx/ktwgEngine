#include "d3d11inputlayout.h"
#include "d3d11device.h"
#include "d3d11renderapi.h"
#include <iostream>

// Define the static resource wrapper table
std::unordered_map<InputLayoutKey, D3D11InputLayout*> DynamicResourceStaticTableWrapper<InputLayoutKey, D3D11InputLayout>::ms_StaticLookupTable;

D3D11InputLayout::D3D11InputLayout(const ComPtr<ID3D11InputLayout>& inputLayout)
:m_InputLayout{inputLayout}
{
  
}

D3D11InputLayout * D3D11InputLayout::GetInputLayout(const InputLayoutKey & key, const ComPtr<ID3DBlob>& compiledShader)
{
  auto& inputLayoutLookupTable = DynamicResourceStaticTableWrapper<InputLayoutKey, D3D11InputLayout>::GetStaticLookupTable();
  auto it = inputLayoutLookupTable.find(key);
  if(it != inputLayoutLookupTable.end())
    return it->second;
  // Otherwise create a new input layout
  uint32_t numInputElements = key.m_InputElements.size();
  std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;
  inputElementDesc.resize(numInputElements);

  for (uint32_t i = 0; i < numInputElements; ++i)
  {
    D3D11_INPUT_ELEMENT_DESC& desc = inputElementDesc[i];
    desc.Format = key.m_InputElements[i].m_Format;
    desc.InputSlot = key.m_InputElements[i].m_InputSlot;
    desc.InstanceDataStepRate = key.m_InputElements[i].m_InstanceDataStepRate;
    desc.InputSlotClass = desc.InstanceDataStepRate ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
    desc.SemanticName = ConvertToSemanticName(key.m_InputElements[i].m_Semantic);
    desc.SemanticIndex = key.m_InputElements[i].m_SemanticIndex;
    desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  }
  ComPtr<ID3D11InputLayout> inputLayout;
  HRESULT hr = D3D11RenderAPI::GetInstance().GetDevice()->GetDevice()->CreateInputLayout(inputElementDesc.data(), numInputElements, compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), inputLayout.GetAddressOf());
  if (FAILED(hr))
  {
    std::cerr << "Failed to create input layout!\n";
    return nullptr;
  }
  D3D11InputLayout* d3d11InputLayout = new D3D11InputLayout{inputLayout};
  inputLayoutLookupTable.emplace(key, d3d11InputLayout);
  return d3d11InputLayout;
}

const char * ConvertToSemanticName(INPUT_ELEMENT_SEMANTIC inputSemantic)
{
  switch (inputSemantic)
  {
  case POSITION:
    return "POSITION";
  case NORMAL:
    return "NORMAL";
  case COLOR:
    return "COLOR";
  }
  return "";
}
