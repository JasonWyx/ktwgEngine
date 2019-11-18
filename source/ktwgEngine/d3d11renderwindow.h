#pragma once

#include <d3d11.h>
#include "d3d11defines.h"
#include "win32window.h"

class D3D11Device;

class D3D11RenderWindow
{
public:
  D3D11RenderWindow(const RENDER_WINDOW_DESC& desc, D3D11Device* device);
  D3D11RenderWindow(const D3D11RenderWindow&)             = delete;
  D3D11RenderWindow(D3D11RenderWindow&&)                  = delete;
  D3D11RenderWindow& operator=(const D3D11RenderWindow&)  = delete;
  D3D11RenderWindow& operator=(D3D11RenderWindow&&)       = delete;
  ~D3D11RenderWindow();

  void Init                             ();
  void CreateSwapChain                  ();
  void CreateSizeDependentResources     ();
  void DestroySizeDependentResources    ();

  void Resize(uint32_t width, uint32_t height);
  void Move(uint32_t left, uint32_t top);
  void Minimize();
  void Maximize();
  void SetHidden(bool hidden);
  void SetFullscreen();
  void SetWindowed(uint32_t width, uint32_t height);
  void SwapBuffers();

  const ComPtr<ID3D11RenderTargetView>& GetRenderTargetView   () const { return m_RenderTargetView; }
  const ComPtr<ID3D11Texture2D>&        GetBackbuffer         () const { return m_Backbuffer; }
  Win32Window*                          GetWindow             () const { return m_Window; }
  
private:
  Win32Window*                    m_Window;
  D3D11Device*                    m_Device;
  ComPtr<IDXGISwapChain>          m_SwapChain;
  ComPtr<ID3D11Texture2D>         m_Backbuffer;
  ComPtr<ID3D11RenderTargetView>  m_RenderTargetView;
  ComPtr<IDXGIFactory>            m_DxgiFactory;
  ComPtr<IDXGIDevice>             m_DxgiDevice;
  RENDER_WINDOW_DESC              m_Desc;
  DXGI_SAMPLE_DESC                m_MultisampleType;
};
