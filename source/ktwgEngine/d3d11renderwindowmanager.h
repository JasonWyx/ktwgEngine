#pragma once

#include "singleton.h"
#include "d3d11defines.h"

class D3D11RenderWindow;

class D3D11RenderWindowManager : public Singleton<D3D11RenderWindowManager>
{
public:
  void InitializeInternal () override;
  void ShutdownInternal   () override;

  D3D11RenderWindow* CreatePrimaryRenderWindow (const RENDER_WINDOW_DESC& desc);
  D3D11RenderWindow* GetPrimaryWindow     () const { return m_PrimaryWindow; }

private:
  D3D11RenderWindow* m_PrimaryWindow;
};
