#include "application.h"

#include "d3d11renderapi.h"
#include "d3d11renderwindow.h"
#include "d3d11renderwindowmanager.h"
#include "d3d11shader.h"
#include "Shaders/cpp/ShaderCommon.h"

#include <fstream>

void Application::InitializeInternal()
{
  InitializeCoreSystems();
  InitializeResources();
  LateInitialize();

  std::fstream f{"Shaders/hlsl/SimpleForward.hlsl"};

  SHADER_DESC desc;
  desc.m_Entry    = "Shade_Vertex";
  desc.m_Source   = std::string{ std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{} };
  desc.m_Type     = VS;
  SimpleForwardVS = D3D11Shader::CreateShader(desc);
  desc.m_Entry    = "Shade_Pixel";
  desc.m_Type     = PS;
  SimpleForwardPS = D3D11Shader::CreateShader(desc);

  if (SimpleForwardVS && SimpleForwardPS)
  {
    std::cout << "Success!\n";
  }
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
