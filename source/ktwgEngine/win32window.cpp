#include "win32window.h"
#include "win32platform.h"
#include <iostream>

Win32Window::Win32Window(const std::string & title, int32_t left, int32_t top, uint32_t width, uint32_t height, HICON icon, bool fullscreen, bool /*hidden*/, bool showTitle)
{
  HINSTANCE hInst = NULL;
  hInst = (HINSTANCE)GetModuleHandle(NULL);
  m_HWNDStyle = 0;
  m_HWNDStyleEx = 0;

  CHAR szExePath[MAX_PATH];
  GetModuleFileName(NULL, szExePath, MAX_PATH);

  // If icon is NULL, choose first icon found in the executable path
  if (icon == NULL)
    icon = ExtractIcon(hInst, szExePath, 0);

  POINT pt;
  pt.x = left;
  pt.y = top;
  m_Monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

  MONITORINFO monitorInfo;
  ZeroMemory(&monitorInfo, sizeof(MONITORINFO));
  monitorInfo.cbSize = sizeof(MONITORINFO);
  GetMonitorInfo(m_Monitor, &monitorInfo);

  int32_t screenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
  int32_t screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

  if (left == -1 || top == -1)
  {
    // Clamp to window dimensions
    int outerWidth = static_cast<int>(width) > screenWidth ? screenWidth : static_cast<int>(width);
    int outerHeight = static_cast<int>(height) > screenHeight ? screenHeight : static_cast<int>(height);

    left = left == -1 ? ((screenWidth - outerWidth) >> 1) : left;
    top = top == -1 ? ((screenHeight - outerHeight) >> 1) : top;
  }

  left += monitorInfo.rcMonitor.left;
  top += monitorInfo.rcMonitor.top;

  WNDCLASS wndClass;
  wndClass.style          = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc    = Win32Platform::Win32WndProc;
  wndClass.cbClsExtra     = 0;
  wndClass.cbWndExtra     = 0;
  wndClass.hInstance      = hInst;
  wndClass.hIcon          = icon;
  wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wndClass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndClass.lpszMenuName   = NULL;
  wndClass.lpszClassName  = TEXT("Win32Window");

  if (!RegisterClass(&wndClass))
  {
    DWORD error = GetLastError();
    UNREFERENCED_PARAMETER(error);
  }

  DWORD windowStyle = WS_OVERLAPPEDWINDOW;

  if (fullscreen)
  {
    windowStyle = WS_POPUP;
    left = 0;
    top = 0;
    width = screenWidth;
    height = screenHeight;
  }
  else
  {
    if (!showTitle)
      windowStyle = WS_POPUP | WS_BORDER;
  }
  SetRect(&m_Rect, left, top, left + width, top + height);
  AdjustWindowRect(&m_Rect, windowStyle, false);

  m_Hwnd = CreateWindow(TEXT("Win32Window"), title.c_str(), windowStyle, m_Rect.left, m_Rect.top, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, NULL, NULL, hInst, NULL);

  if (!m_Hwnd)
  {
    DWORD error = GetLastError();
    std::cerr << error << "\n";
    exit(1);
  }
}

Win32Window::~Win32Window()
{
  if (m_Hwnd)
  {
    DestroyWindow(m_Hwnd);
  }
}


void Win32Window::Resize(uint32_t width, uint32_t height)
{
  m_Rect.right = m_Rect.left + width;
  m_Rect.bottom = m_Rect.top + height;
}

void Win32Window::Move(int32_t left, int32_t top)
{
  m_Rect.right += left - m_Rect.left;
  m_Rect.bottom += top - m_Rect.top;
  m_Rect.left = left;
  m_Rect.top = top;
}

void Win32Window::SetFullscreen(bool fullscreen)
{
  if (fullscreen)
  {
    m_Fullscreen = FALSE;
    GetWindowPlacement(m_Hwnd, &m_Wpc);
    m_HWNDStyle = GetWindowLong(m_Hwnd, GWL_STYLE);
    m_HWNDStyleEx = GetWindowLong(m_Hwnd, GWL_EXSTYLE);

    auto HWNDStyle = m_HWNDStyle;
    HWNDStyle &= ~WS_BORDER;
    HWNDStyle &= ~WS_DLGFRAME;
    HWNDStyle &= ~WS_THICKFRAME;

    auto HWNDStyleEx = m_HWNDStyleEx;
    HWNDStyleEx &= ~WS_EX_WINDOWEDGE;
    SetWindowLong(m_Hwnd, GWL_STYLE, HWNDStyle | WS_POPUP);
    SetWindowLong(m_Hwnd, GWL_EXSTYLE, HWNDStyleEx | WS_EX_TOPMOST);
    ShowWindow(m_Hwnd, SW_SHOWMAXIMIZED);

    HMONITOR hMon = MonitorFromWindow(m_Hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };

    GetMonitorInfo(hMon, &mi);
    RECT windowRect{ mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom };
    SetWindowPos(m_Hwnd, NULL, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    // Resize(mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
  }
  else
  {
    m_Fullscreen = TRUE;
    SetWindowLong(m_Hwnd, GWL_STYLE, m_HWNDStyle);
    SetWindowLong(m_Hwnd, GWL_EXSTYLE, m_HWNDStyleEx);
    ShowWindow(m_Hwnd, SW_SHOWNORMAL);
    SetWindowPlacement(m_Hwnd, &m_Wpc);
  }
}

void Win32Window::SetHidden(bool hidden)
{
  ShowWindow(m_Hwnd, hidden ? SW_HIDE : SW_SHOW);
  m_Hidden = hidden;
}

void Win32Window::Minimize()
{
  if (m_Hwnd)
  {
    ShowWindow(m_Hwnd, SW_MINIMIZE);
  }

  if (m_Hidden)
  {
    ShowWindow(m_Hwnd, SW_HIDE);
  }
}

void Win32Window::Maximize()
{
  if (m_Hwnd)
  {
    ShowWindow(m_Hwnd, SW_MAXIMIZE);
  }

  if (m_Hidden)
  {
    ShowWindow(m_Hwnd, SW_HIDE);
  }
}

void Win32Window::Restore()
{
  if (m_Hwnd)
  {
    ShowWindow(m_Hwnd, SW_RESTORE);
  }

  if (m_Hidden)
  {
    ShowWindow(m_Hwnd, SW_HIDE);
  }
}
