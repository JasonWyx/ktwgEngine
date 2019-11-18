#include "d3d11renderwindow.h"
#include "d3d11renderapi.h"

int main()
{
  D3D11RenderAPI::Initialize();

  RENDER_WINDOW_DESC desc;
  desc.m_VideoMode = { 1280, 600, 60.f };
  desc.m_Title = "TestWindow";
  desc.m_Fullscreen = false;
  desc.m_Hidden = false;
  desc.m_ShowBorder = true;
  desc.m_ShowTitleBar = true;
  desc.m_Left = -1;
  desc.m_Top = -1;
  desc.m_MultisampleCount = 1;
  D3D11RenderWindow* window = new D3D11RenderWindow{desc, D3D11RenderAPI::GetInstancePtr()->GetDevice()};
  window->Init();
  window->SetHidden(false);
  while (true)
  {

  }
  return 0;
}