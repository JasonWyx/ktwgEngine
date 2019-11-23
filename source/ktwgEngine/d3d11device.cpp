#include "d3d11device.h"
#include "d3d11texture.h"
#include "d3d11shader.h"
#include "d3d11hardwarebuffer.h"
#include <vector>

#if _DEBUG
#include <cassert>
#endif // #if _DEBUG

D3D11Device::D3D11Device(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
:m_Device{device}, m_ImmediateContext{context}, m_InfoQueue{nullptr}
{
#if defined(ENABLEDEBUGLAYER)
  HRESULT hr = device_->QueryInterface(__uuidof(ID3D11InfoQueue), &m_InfoQueue);
  if (FAILED(hr))
  {
    LOG("Failed to enable ID3D11InfoQueue!");
  }
  SetExceptionsErrorLevel(D3D11_EL_ERROR);
#endif
}

D3D11Device::~D3D11Device()
{
  
}

std::string D3D11Device::GetErrors(bool clearErrors)
{
  std::string result;

#if defined(ENABLEDEBUGLAYER)
  if (m_InfoQueue)
  {
    uint64_t numMessages = m_InfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
    for (auto i = 0U; i < numMessages; ++i)
    {
      SIZE_T length = 0;
      m_InfoQueue->GetMessage(i, nullptr, &length);
      D3D11_MESSAGE* message = reinterpret_cast<D3D11_MESSAGE*>(new uint8_t[static_cast<unsigned>(length)]);
      m_InfoQueue->GetMessage(i, message, &length);
      result += message->pDescription;
      result += "\n";
      delete [] reinterpret_cast<uint8_t*>(message);
    }
    if (clearErrors)
      ClearErrors();
  }
#endif
  UNREFERENCED_PARAMETER(clearErrors);

  return result;
}

void D3D11Device::ClearErrors()
{
#if defined(ENABLEDEBUGLAYER)
  if (m_InfoQueue)
  {
    m_InfoQueue->ClearStoredMessages();
  }
#endif
}

void D3D11Device::SetExceptionsErrorLevel(D3D11_ERROR_LEVEL errorLevel)
{
#if defined(ENABLEDEBUGLAYER)
  if (m_InfoQueue)
  {
    m_InfoQueue->ClearRetrievalFilter();
    m_InfoQueue->ClearStorageFilter();

    D3D11_INFO_QUEUE_FILTER filter;
    ZeroMemory(&filter, sizeof(D3D11_INFO_QUEUE_FILTER));
    std::vector<D3D11_MESSAGE_SEVERITY> severity;
    switch (errorLevel)
    {
    case D3D11_EL_INFO:
      severity.push_back(D3D11_MESSAGE_SEVERITY_INFO);
    case D3D11_EL_WARNING:
      severity.push_back(D3D11_MESSAGE_SEVERITY_WARNING);
    case D3D11_EL_ERROR:
      severity.push_back(D3D11_MESSAGE_SEVERITY_ERROR);
    case D3D11_EL_CORRUPTION:
      severity.push_back(D3D11_MESSAGE_SEVERITY_CORRUPTION);
    case D3D11_EL_NO_EXCEP:
    default:
      break;
    }
    if (severity.size() > 0)
    {
      filter.AllowList.NumSeverities = static_cast<UINT>(severity.size());
      filter.AllowList.pSeverityList = &severity[0];
    }
    m_InfoQueue->AddRetrievalFilterEntries(&filter);
    m_InfoQueue->AddStorageFilterEntries(&filter);
  }
#endif
  UNREFERENCED_PARAMETER(errorLevel);
}

bool D3D11Device::HasError() const
{
#if defined(ENABLEDEBUGLAYER)
  if (m_InfoQueue)
  {
    return m_InfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter() > 0;
  }
#endif
  return false;
}

D3D11Context::D3D11Context(const ComPtr<ID3D11DeviceContext>& context)
:m_Context{context}
{
}

D3D11Context::~D3D11Context()
{
  if (m_Context)
  {
    m_Context->Flush();
    m_Context->ClearState();
  }
}

void D3D11Context::ClearRTV(D3D11Texture * renderTarget, DXGI_FORMAT format, float r, float g, float b, float a)
{
  ResourceViewKey key;
  key.m_Type = RTV;
  key.m_Format = format;
  key.m_MostDetailedMip = 0;
  float rgba[4] = {r, g, b, a};
  m_Context->ClearRenderTargetView(renderTarget->GetView(key).GetRtv(), rgba);
}

void D3D11Context::ClearDSV(D3D11Texture * depthStencil, DXGI_FORMAT format, UINT clearFlags, float d, uint8_t stencil)
{
  ResourceViewKey key;
  key.m_Type = DSV;
  key.m_Format = format;
  key.m_MostDetailedMip = 0;
  m_Context->ClearDepthStencilView(depthStencil->GetView(key).GetDsv(), clearFlags, d, stencil);
}

void D3D11Context::AddRenderTarget(D3D11Texture * renderTarget, DXGI_FORMAT format)
{
#if _DEBUG
  assert(m_RenderTargets.size() < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT - 1);
#endif // #if _DEBUG
  ResourceViewKey key;
  key.m_Type = RTV;
  key.m_Format = format;
  key.m_MostDetailedMip = 0;
  m_RenderTargets.emplace_back(renderTarget->GetView(key).GetRtv());
}

void D3D11Context::SetDepthStencil(D3D11Texture * depthStencil, DXGI_FORMAT format)
{
  ResourceViewKey key;
  key.m_Type = DSV;
  key.m_Format = format;
  key.m_MostDetailedMip = 0;
  m_DepthStencil = depthStencil->GetView(key).GetDsv();
}

void D3D11Context::FlushRenderTargets()
{
  m_Context->OMSetRenderTargets((UINT)m_RenderTargets.size(), m_RenderTargets.data(), m_DepthStencil);
  m_RenderTargets.clear();
  m_DepthStencil = nullptr;
}

void D3D11Context::AddVertexBuffer(D3D11HardwareBuffer * vertexBuffer, uint32_t stride, uint32_t offset)
{
  m_VertexBuffers.emplace_back(vertexBuffer->GetBuffer());
  m_VertexStrides.emplace_back(stride);
  m_VertexOffsets.emplace_back(offset);
}

void D3D11Context::FlushVertexBuffers()
{
  m_Context->IASetVertexBuffers(0, (UINT)m_VertexBuffers.size(), m_VertexBuffers.data(), m_VertexStrides.data(), m_VertexOffsets.data());
  m_VertexBuffers.clear();
  m_VertexStrides.clear();
  m_VertexOffsets.clear();
}

void D3D11Context::SetIndexBuffer(D3D11HardwareBuffer * indexBuffer, DXGI_FORMAT format, uint32_t offset)
{
  m_Context->IASetIndexBuffer(indexBuffer->GetBuffer(), format, offset);
}

void D3D11Context::Set(D3D11VertexShader * vs)
{
  m_Context->VSSetShader(vs->GetShader().Get(), NULL, 0);
}

void D3D11Context::Set(D3D11PixelShader * ps)
{
  m_Context->PSSetShader(ps->GetShader().Get(), NULL, 0);
}

void D3D11Context::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t numIndices, uint32_t startIndexLocation, int32_t baseVertexLocation)
{
  m_Context->IASetPrimitiveTopology(topology);
  m_Context->DrawIndexed(numIndices, startIndexLocation, baseVertexLocation);
}

void D3D11Context::FlushConstantBuffers(uint32_t startSlot)
{
  if(m_VSConstantBuffers.size())
    m_Context->VSSetConstantBuffers(startSlot, (UINT)m_VSConstantBuffers.size(), m_VSConstantBuffers.data());
  if(m_PSConstantBuffers.size())
    m_Context->PSSetConstantBuffers(startSlot, (UINT)m_PSConstantBuffers.size(), m_VSConstantBuffers.data());
}
