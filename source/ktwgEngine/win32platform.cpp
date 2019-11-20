#include "win32platform.h"
#include "inputsystem.h"

LRESULT Win32Platform::Input_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return 0;
}

LRESULT Win32Platform::Win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (Input_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  InputSystem& inputSys = InputSystem::GetInstance();

  switch (uMsg)
  {
  case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
  case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
  case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
  {
    int button = 0;
    if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) button = 0;
    if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK) button = 1;
    if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK) button = 2;
    inputSys.m_Mouse[button] = true;
    return 0;
  }
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
  {
    int button = 0;
    if (uMsg == WM_LBUTTONUP) button = 0;
    if (uMsg == WM_RBUTTONUP) button = 1;
    if (uMsg == WM_MBUTTONUP) button = 2;
    inputSys.m_Mouse[button] = false;
    return 0;
  }
  case WM_MOUSEWHEEL:
    inputSys.m_MouseWheel += (double)GET_WHEEL_DELTA_WPARAM(wParam) / (double)WHEEL_DELTA;
    return 0;
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    if (wParam < KEY_BUFFER_SIZE) inputSys.MapKeyDown(wParam);
    return 0;
  case WM_KEYUP:
  case WM_SYSKEYUP:
    if (wParam < KEY_BUFFER_SIZE) inputSys.MapKeyUp(wParam);
    return 0;
  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}