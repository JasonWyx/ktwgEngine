#include "d3d11renderwindow.h"
#include "d3d11renderapi.h"
#include "d3d11device.h"
#include <iostream>

D3D11RenderWindow::D3D11RenderWindow(const RENDER_WINDOW_DESC & desc, D3D11Device * device)
:m_Desc{desc}, m_Device{device}, m_Window{nullptr}, m_SwapChain{nullptr}
{
}

D3D11RenderWindow::~D3D11RenderWindow()
{
  if (m_Window)
  {
    delete m_Window;
  }
}

void D3D11RenderWindow::Init()
{
  m_MultisampleType.Count = 1;
  m_MultisampleType.Quality = 0;

  m_Window = new Win32Window{m_Desc.m_Title, m_Desc.m_Left, m_Desc.m_Top, m_Desc.m_VideoMode.m_Width, m_Desc.m_VideoMode.m_Height, (HICON)NULL, m_Desc.m_Fullscreen, m_Desc.m_Hidden, m_Desc.m_ShowTitleBar};

  // Query IDXGIFactory
  D3D11RenderAPI* renderAPI = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::GetInstancePtr());
  HRESULT hr = renderAPI->GetDevice()->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_DxgiDevice);

  ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
  hr = m_DxgiDevice->GetAdapter(&dxgiAdapter);

  dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&m_DxgiFactory);

  CreateSwapChain();

  m_SwapChain->SetFullscreenState(m_Desc.m_Fullscreen, NULL);

  CreateSizeDependentResources();

  m_DxgiFactory->MakeWindowAssociation(m_Window->GetHwnd(), 0);
}

void D3D11RenderWindow::CreateSwapChain()
{
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

  swapChainDesc.OutputWindow = m_Window->GetHwnd();
  swapChainDesc.BufferDesc.Width = m_Window->GetWidth();
  swapChainDesc.BufferDesc.Height = m_Window->GetHeight();
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
  swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 1;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  swapChainDesc.Windowed = true;
  swapChainDesc.SampleDesc.Count = m_MultisampleType.Count;
  swapChainDesc.SampleDesc.Quality = m_MultisampleType.Quality;

  HRESULT hr = m_DxgiFactory->CreateSwapChain(m_DxgiDevice.Get(), &swapChainDesc, &m_SwapChain);

  if (FAILED(hr))
  {
    // Try again if failed, may fail the first time, but will be corrected 
    hr = m_DxgiFactory->CreateSwapChain(m_DxgiDevice.Get(), &swapChainDesc, &m_SwapChain);
    if (FAILED(hr))
    {
      std::cerr << "Failed to create swap chain\n";
      exit(1);
    }
  }
}

void D3D11RenderWindow::CreateSizeDependentResources()
{
  // Release the backBuffer
  m_Backbuffer.Reset();

  HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_Backbuffer);

  if (FAILED(hr))
  {
    std::cerr << "Failed to Get Buffer!\n";
    exit(1);
  }

  D3D11_TEXTURE2D_DESC backBufferDesc;
  m_Backbuffer->GetDesc(&backBufferDesc);

  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
  ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

  rtvDesc.Format = backBufferDesc.Format;
  rtvDesc.ViewDimension = m_Desc.m_MultisampleCount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
  rtvDesc.Texture2D.MipSlice = 0;

  hr = m_Device->GetDevice()->CreateRenderTargetView(m_Backbuffer.Get(), &rtvDesc, m_RenderTargetView.GetAddressOf());

  if (FAILED(hr))
  {
    std::cerr << "Failed to create render target view!\n";
    exit(1);
  }
}

void D3D11RenderWindow::DestroySizeDependentResources()
{
  m_Device->GetImmediateContext().GetContext()->OMSetRenderTargets(0, 0, 0);
  m_RenderTargetView.Reset();
  m_Backbuffer.Reset();
}

void D3D11RenderWindow::Resize(uint32_t width, uint32_t height)
{
  DestroySizeDependentResources();
  HRESULT hr = m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

  if (FAILED(hr))
  {
    auto msg = m_Device->GetErrors();
    std::cerr << msg << "\n";
    return;
  }

  CreateSizeDependentResources();

  m_Device->GetImmediateContext().GetContext()->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), NULL);

  m_Window->Resize(width, height);

  D3D11_VIEWPORT viewport;
  viewport.Height = (FLOAT)height;
  viewport.Width = (FLOAT)width;
  viewport.MinDepth = 0.f;
  viewport.MaxDepth = 1.f;
  viewport.TopLeftX = 0.f;
  viewport.TopLeftY = 0.f;
  m_Device->GetImmediateContext().GetContext()->RSSetViewports(1, &viewport);
}

void D3D11RenderWindow::Move(uint32_t left, uint32_t top)
{
  m_Desc.m_Left = left;
  m_Desc.m_Top = top;
  m_Window->Move(left, top);
}

void D3D11RenderWindow::Minimize()
{
  m_Window->Minimize();
}

void D3D11RenderWindow::Maximize()
{
  m_Window->Maximize();
}

void D3D11RenderWindow::SetHidden(bool hidden)
{
  m_Window->SetHidden(hidden);
  m_Desc.m_Hidden = hidden;
}

void D3D11RenderWindow::SetFullscreen()
{
  m_Window->SetFullscreen(true);
}

void D3D11RenderWindow::SetWindowed(uint32_t /*width*/, uint32_t /*height*/)
{
  m_Window->SetFullscreen(false);
}

void D3D11RenderWindow::SwapBuffers()
{
  m_SwapChain->Present(0, 0);
}
