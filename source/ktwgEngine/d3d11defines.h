#pragma once

#include <wrl.h>
#include <cinttypes>
#include <string>

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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

enum D3D11_BUFFER_TYPE
{
  D3D11_BT_VERTEX,
  D3D11_BT_INDEX,
  D3D11_BT_CONSTANT,
  D3D11_BT_STRUCTURED,
  D3D11_BT_RAW,
  D3D11_BT_Count
};

enum GPU_LOCK_OPTIONS
{
  LO_READ_WRITE, // Read and write, Might limit performance

  LO_READ_ONLY, // Allows reading (SLOW OPERATION)

  LO_WRITE_ONLY_DISCARD, // Write to buffer, driver discards contents 
  LO_WRITE_ONLY_DISCARD_RANGE, // Write to buffer, driver discards contents of a range (not entire buffer)
  LO_WRITE_ONLY_NO_OVERWRITE, // Write to buffer, guarantees the driver will not update any part that is currently used
  LO_WRITE_ONLY // Write to buffer
};

enum GPU_WRITE_TYPE
{
  WT_NORMAL,
  WT_DISCARD,
  WT_NO_DISCARD,
  WT_NO_OVERWRITE
};

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
