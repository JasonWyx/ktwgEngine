#include "win32platform.h"

LRESULT Win32Platform::Input_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return 0;
}

LRESULT Win32Platform::Win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (Input_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  switch (uMsg)
  {
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