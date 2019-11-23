#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "d3d11defines.h"
#include "matrix4.h"

class D3D11Texture;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11HardwareBuffer;

class D3D11Context
{
public:
  D3D11Context(const ComPtr<ID3D11DeviceContext>& context);
  ~D3D11Context();

  ComPtr<ID3D11DeviceContext>& GetContext() { return m_Context; }
  const ComPtr<ID3D11DeviceContext>& GetContext() const { return m_Context; }

  void ClearRTV(D3D11Texture* renderTarget, DXGI_FORMAT format, float r, float g, float b, float a);
  void ClearDSV(D3D11Texture* depthStencil, DXGI_FORMAT format, UINT clearFlags, float d, uint8_t stencil);

  void AddRenderTarget(D3D11Texture* renderTarget, DXGI_FORMAT format);
  void SetDepthStencil(D3D11Texture* depthStencil, DXGI_FORMAT format);
  void FlushRenderTargets();

  void AddVertexBuffer(D3D11HardwareBuffer* vertexBuffer, uint32_t stride, uint32_t offset);
  void FlushVertexBuffers();

  void SetIndexBuffer(D3D11HardwareBuffer* indexBuffer, DXGI_FORMAT format, uint32_t offset);

  void Set(D3D11VertexShader* vs);
  void Set(D3D11PixelShader* ps);

  void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t numIndices, uint32_t startIndexLocation, int32_t baseVertexLocation);

  template <SHADER_TYPE stage>
  void AddConstantBuffer(D3D11HardwareBuffer* buffer);

  template <>
  void AddConstantBuffer<VS>(D3D11HardwareBuffer* buffer);

  template <>
  void AddConstantBuffer<PS>(D3D11HardwareBuffer* buffer);

  void FlushConstantBuffers(uint32_t startSlot);

  void Set(const Matrix4& view, const Matrix4& proj, const Matrix4& viewProj) { m_View = view; m_Proj = proj; m_ViewProj = viewProj; }

  const Matrix4& GetView() const { return m_View; }
  const Matrix4& GetProj() const { return m_Proj; }
  const Matrix4& GetViewProj() const { return m_ViewProj; }

private:
  std::vector<ID3D11RenderTargetView*>    m_RenderTargets;
  ID3D11DepthStencilView*                 m_DepthStencil;

  std::vector<ID3D11Buffer*>              m_VertexBuffers;
  std::vector<UINT>                       m_VertexStrides;
  std::vector<UINT>                       m_VertexOffsets;

  ID3D11Buffer*                           m_IndexBuffer;

  std::vector<ID3D11Buffer*>              m_VSConstantBuffers;
  std::vector<ID3D11Buffer*>              m_PSConstantBuffers;

  ComPtr<ID3D11DeviceContext>             m_Context;

  Matrix4                                 m_View;
  Matrix4                                 m_Proj;
  Matrix4                                 m_ViewProj;
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

#include "d3d11device.hpp"
