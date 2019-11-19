#pragma once

#include <wrl.h>
#include <cinttypes>
#include <string>

struct VideoMode
{
  VideoMode() = default;

  VideoMode(uint32_t width, uint32_t height, float refreshRate)
  :m_Width{width}, m_Height{height}, m_RefreshRate{refreshRate}
  {
  }

  uint32_t GetWidth           () const { return m_Width; }
  uint32_t GetHeight          () const { return m_Height; }
  float    GetRefreshRate     () const { return m_RefreshRate; }

  uint32_t  m_Width       = 1280;
  uint32_t  m_Height      = 720;
  float     m_RefreshRate = 60.0f;
};

struct RENDER_WINDOW_DESC
{
  VideoMode   m_VideoMode;
  uint32_t    m_VSyncInterval;
  uint32_t    m_MultisampleCount;
  int32_t     m_Left;
  int32_t     m_Top;
  std::string m_Title;
  bool        m_Fullscreen;
  bool        m_Hidden;
  bool        m_VSync;
  bool        m_ShowTitleBar;
  bool        m_ShowBorder;
};

// E.g D3D11_EL_NO_EXCEP
enum D3D11_ERROR_LEVEL
{
  D3D11_EL_NO_EXCEP,
  D3D11_EL_CORRUPTION,
  D3D11_EL_ERROR,
  D3D11_EL_WARNING,
  D3D11_EL_INFO
};

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
