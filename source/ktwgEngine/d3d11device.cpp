#include "d3d11device.h"
#include <vector>

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
  if (m_ImmediateContext)
  {
    m_ImmediateContext->Flush();
    m_ImmediateContext->ClearState();
  }
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
