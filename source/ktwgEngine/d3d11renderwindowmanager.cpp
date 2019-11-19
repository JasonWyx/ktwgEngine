#include "d3d11renderwindowmanager.h"
#include "d3d11renderapi.h"
#include "d3d11renderwindow.h"

void D3D11RenderWindowManager::InitializeInternal()
{
  m_PrimaryWindow = nullptr;
}

void D3D11RenderWindowManager::ShutdownInternal()
{
  if (m_PrimaryWindow)
  {
    delete m_PrimaryWindow;
  }
}

D3D11RenderWindow * D3D11RenderWindowManager::CreatePrimaryRenderWindow(const RENDER_WINDOW_DESC & desc)
{
  if (m_PrimaryWindow)
  {
    delete m_PrimaryWindow;
  }
  m_PrimaryWindow = new D3D11RenderWindow {desc, D3D11RenderAPI::GetInstance().GetDevice()};
  return m_PrimaryWindow;
}
