#pragma once

#include <d3d11.h>
#include <vector>
#include "d3d11defines.h"

enum INPUT_ELEMENT_SEMANTIC
{
  POSITION,
  NORMAL,
  COLOR
};

struct InputElement
{
  INPUT_ELEMENT_SEMANTIC  m_Semantic;
  DXGI_FORMAT             m_Format;
};

struct InputLayoutKey
{
  std::vector<InputElement> m_InputElements;
};

bool operator==(const InputElement& lhs, const InputElement& rhs)
{
  return lhs.m_Semantic == rhs.m_Semantic && 
         lhs.m_Format == rhs.m_Format;
}

bool operator==(const InputLayoutKey& lhs, const InputLayoutKey& rhs)
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
      hash_combine(hash, (size_t)val.m_Format);
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

class D3D11InputLayout
{
public:
  D3D11InputLayout(const ComPtr<ID3D11InputLayout>& inputLayout);

  const ComPtr<ID3D11InputLayout>& GetInputLayout() const { return m_InputLayout; }

private:
  ComPtr<ID3D11InputLayout> m_InputLayout;
};
