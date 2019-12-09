#pragma once
#include "win32_includes.h"
#include <string>
#include <cinttypes>

class Win32Window
{
public:
  Win32Window(const std::string& title, int32_t left, int32_t top, uint32_t width, uint32_t height, HICON icon, bool fullscreen, bool hidden, bool showTitle);

  Win32Window(const Win32Window&)             = delete;
  Win32Window(Win32Window&&)                  = delete;
  Win32Window& operator=(const Win32Window&)  = delete;
  Win32Window& operator=(Win32Window&&)       = delete;

  ~Win32Window();

  void Resize         (uint32_t width, uint32_t height);
  void Move           (int32_t left, int32_t top);
  void SetFullscreen  (bool fullscreen);
  void SetHidden      (bool hidden);
  void Maximize       ();
  void Minimize       ();
  void Restore        ();

  int32_t   GetLeft     () const { return m_Rect.left; }
  int32_t   GetTop      () const { return m_Rect.top; }
  uint32_t  GetWidth    () const { return static_cast<uint32_t>(m_Rect.right - m_Rect.left); }
  uint32_t  GetHeight   () const { return static_cast<uint32_t>(m_Rect.bottom - m_Rect.top); }

  RECT      GetRect     () const { return m_Rect; }
  HWND      GetHwnd     () const { return m_Hwnd; }
  HMONITOR  GetMonitor  () const { return m_Monitor; }

private:
  HWND                m_Hwnd;
  RECT                m_Rect;
  HMONITOR            m_Monitor; // Monitor that this window is on
  LONG                m_HWNDStyle;
  LONG                m_HWNDStyleEx;
  WINDOWPLACEMENT     m_Wpc;
  bool                m_Hidden;
  bool                m_Fullscreen;
};