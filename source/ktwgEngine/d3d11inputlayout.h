#pragma once

#include <d3d11.h>
#include <vector>
#include <unordered_map>
#include "d3d11defines.h"

enum INPUT_ELEMENT_SEMANTIC
{
  POSITION,
  NORMAL,
  COLOR
};

const char* ConvertToSemanticName(INPUT_ELEMENT_SEMANTIC inputSemantic);

struct InputElement
{
  INPUT_ELEMENT_SEMANTIC     m_Semantic;
  DXGI_FORMAT                m_Format;
  UINT                       m_SemanticIndex;
  UINT                       m_InputSlot;
  UINT                       m_InstanceDataStepRate;
};

struct InputLayoutKey
{
  std::vector<InputElement> m_InputElements;
  void AddInputElement(INPUT_ELEMENT_SEMANTIC semantic, DXGI_FORMAT format, UINT semanticIndex, UINT inputSlot, UINT instanceDataStepRate)
  {
    InputElement elem;
    elem.m_Semantic = semantic;
    elem.m_Format = format;
    elem.m_SemanticIndex = semanticIndex;
    elem.m_InputSlot = inputSlot;
    elem.m_InstanceDataStepRate = instanceDataStepRate;
    m_InputElements.emplace_back(elem);
  }
};

inline bool operator==(const InputElement& lhs, const InputElement& rhs)
{
  return lhs.m_Semantic == rhs.m_Semantic && 
         lhs.m_Format == rhs.m_Format &&
         lhs.m_InputSlot == rhs.m_InputSlot &&
         lhs.m_InstanceDataStepRate == rhs.m_InstanceDataStepRate &&
         lhs.m_SemanticIndex == rhs.m_SemanticIndex;
}

inline bool operator==(const InputLayoutKey& lhs, const InputLayoutKey& rhs)
{
  return lhs.m_InputElements == rhs.m_InputElements;
}

namespace std
{
  template<>
  struct hash<InputElement>
  {
    size_t operator()(const InputElement& val) const noexcept
    {
      size_t hash;
      hash_combine(hash, (size_t)val.m_Semantic);
      hash_combine(hash, (size_t)val.m_SemanticIndex);
      hash_combine(hash, (size_t)val.m_Format);
      hash_combine(hash, (size_t)val.m_InputSlot);
      hash_combine(hash, (size_t)val.m_InstanceDataStepRate);
      return hash;
    }
  };

  template<>
  struct hash<InputLayoutKey>
  {
    size_t operator()(const InputLayoutKey& val) const noexcept
    {
      size_t hash;
      for (const InputElement& elem : val.m_InputElements)
      {
        hash_combine(hash, elem);
      }
      return hash;
    }
  };
}

template <typename key_t, typename res_t>
class DynamicResourceStaticTableWrapper
{
public:
  ~DynamicResourceStaticTableWrapper()
  {
    for (auto& elem : ms_StaticLookupTable)
    {
      delete elem;
    }
  }

  static std::unordered_map<key_t, res_t*>& GetStaticLookupTable() { return ms_StaticLookupTable; }

private:
  static std::unordered_map<key_t, res_t*> ms_StaticLookupTable;
};

class D3D11InputLayout
{
public:
  D3D11InputLayout(const ComPtr<ID3D11InputLayout>& compiledShader);

  const ComPtr<ID3D11InputLayout>& GetInputLayout() const { return m_InputLayout; }

  static D3D11InputLayout* GetInputLayout(const InputLayoutKey& key, const ComPtr<ID3DBlob>& compiledShader);

private:
  ComPtr<ID3D11InputLayout> m_InputLayout;
};
