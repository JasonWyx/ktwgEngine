#include "application.h"

#include "d3d11renderapi.h"
#include "d3d11renderwindow.h"
#include "d3d11renderwindowmanager.h"

#include "hyperenderer.h"

#include <fstream>

void Application::InitializeInternal()
{
  InitializeCoreSystems();
  InitializeResources();
  LateInitialize();
}

void Application::ShutdownInternal()
{
  D3D11RenderWindowManager::Shutdown();
  D3D11RenderAPI::Shutdown();
}

void Application::Run()
{
  // For now just a loop forever
  while (true)
  {
    
  }
}

void Application::InitializeCoreSystems()
{
  D3D11RenderAPI::Initialize();
  D3D11RenderWindowManager::Initialize();
}

void Application::InitializeResources()
{
  HypeRenderer::LoadSystemShaders();
}

void Application::LateInitialize()
{
  RENDER_WINDOW_DESC desc;
  desc.m_VideoMode          = { 1280, 600, 60.f };
  desc.m_Title              = "TestWindow";
  desc.m_Fullscreen         = false;
  desc.m_Hidden             = false;
  desc.m_ShowBorder         = true;
  desc.m_ShowTitleBar       = true;
  desc.m_Left               = -1;
  desc.m_Top                = -1;
  desc.m_MultisampleCount   = 1;

  D3D11RenderWindow* renderWindow = D3D11RenderWindowManager::GetInstance().CreatePrimaryRenderWindow(desc);
  renderWindow->Init();
  renderWindow->SetHidden(false);
}
