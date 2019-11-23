#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "d3d11defines.h"

class D3D11Texture;
class D3D11VertexShader;
class D3D11PixelShader;

class D3D11Context
{
public:
  D3D11Context(const ComPtr<ID3D11DeviceContext>& context);
  ~D3D11Context();

  ComPtr<ID3D11DeviceContext>& GetContext() { return m_Context; }
  const ComPtr<ID3D11DeviceContext>& GetContext() const { return m_Context; }
  void AddRenderTarget(D3D11Texture* renderTarget, DXGI_FORMAT format);
  void SetDepthStencil(D3D11Texture* depthStencil, DXGI_FORMAT format);
  void FlushRenderTargets();

  void Set(D3D11VertexShader* vs);
  void Set(D3D11PixelShader* ps);

private:
  std::vector<ID3D11RenderTargetView*>    m_RenderTargets;
  ID3D11DepthStencilView*                 m_DepthStencil;
  ComPtr<ID3D11DeviceContext>             m_Context;
};

class D3D11Device
{
public:
  D3D11Device(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
  ~D3D11Device();

  const ComPtr<ID3D11Device>&     GetDevice           () const { return m_Device; }
  ComPtr<ID3D11Device>&           GetDevice           ()       { return m_Device; }
  D3D11Context&                   GetImmediateContext ()       { return m_ImmediateContext; }
  const D3D11Context&             GetImmediateContext () const { return m_ImmediateContext; }
  ComPtr<ID3D11InfoQueue>         GetInfoQueue        () const { return m_InfoQueue; }

  std::string GetErrors                 (bool clearErrors = true);
  void        ClearErrors               ();
  void        SetExceptionsErrorLevel   (D3D11_ERROR_LEVEL errorLevel);
  bool        HasError                  () const;

private:
  ComPtr<ID3D11Device>        m_Device;
  D3D11Context                m_ImmediateContext;
  ComPtr<ID3D11InfoQueue>     m_InfoQueue;
};
