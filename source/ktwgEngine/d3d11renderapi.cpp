#include "d3d11renderapi.h"
#include <wrl.h>
#include <iostream>
#include "d3d11defines.h"
#include "d3d11device.h"

void D3D11RenderAPI::InitializeInternal()
{
  // Only support dx11
  D3D_FEATURE_LEVEL levels[] =
  {
    D3D_FEATURE_LEVEL_11_0
  };
  
  ComPtr<ID3D11Device> device;
  ComPtr<ID3D11DeviceContext> immediateContext;
  
  UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(ENABLEDEBUGLAYER)
  deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // Specify nullptr to use the default adapter
  HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlags, levels, sizeof(levels) / sizeof(levels[0]), D3D11_SDK_VERSION, &device, &m_FeatureLevel, &immediateContext);

  if (FAILED(hr))
  {
    std::cerr << "Failed to initialize D3D11\n";
    exit(1);
  }

  m_Device = new D3D11Device{device, immediateContext};
}

void D3D11RenderAPI::ShutdownInternal()
{
  delete m_Device;
}
