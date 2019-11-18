#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include "d3d11defines.h"

class D3D11Device
{
public:
  D3D11Device(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
  ~D3D11Device();

  ComPtr<ID3D11Device>            GetDevice           () const { return m_Device; }
  ComPtr<ID3D11DeviceContext>     GetImmediateContext () const { return m_ImmediateContext; }
  ComPtr<ID3D11InfoQueue>         GetInfoQueue        () const { return m_InfoQueue; }

  std::string GetErrors                 (bool clearErrors = true);
  void        ClearErrors               ();
  void        SetExceptionsErrorLevel   (D3D11_ERROR_LEVEL errorLevel);
  bool        HasError                  () const;

private:
  ComPtr<ID3D11Device>        m_Device;
  ComPtr<ID3D11DeviceContext> m_ImmediateContext;
  ComPtr<ID3D11InfoQueue>     m_InfoQueue;
};
