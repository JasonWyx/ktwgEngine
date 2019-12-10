/******************************************************************************/
/*!
\file       Win32Platform.h
\author     Low Jin Liang Aaron
\par        Project: BPEngine
\brief      This is the interface file for Win32Platform.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

#include "win32_includes.h"
#include <windowsx.h>

// --------- Class that implements the operation on a Win32 Platform ------------ //
class Win32Platform
{
public:
  static bool m_ShowCursor;
  static bool m_LockCursor;

  static LRESULT Input_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK Win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
