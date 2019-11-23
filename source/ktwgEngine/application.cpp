#include "application.h"

#include <iostream>

#include "d3d11renderapi.h"
#include "d3d11renderwindow.h"
#include "d3d11renderwindowmanager.h"

#include "hyperenderer.h"

#include <fstream>
#include "time.h"
#include "inputsystem.h"
#include "physics.h"

#include "scene.h"

void Application::InitializeInternal()
{
  InitializeCoreSystems();
  InitializeResources();
  LateInitialize();
}

void Application::ShutdownInternal()
{
  Scene::Shutdown();

  Physics::Shutdown();

  InputSystem::Shutdown();

  D3D11RenderWindowManager::Shutdown();
  D3D11RenderAPI::Shutdown();

  Time::Shutdown();
}

void Application::Run()
{
  MSG msg = { 0 };

  Time& time = Time::GetInstance();
  time.Update();

  // Accumulate the frame time
  double accumulator = 0.0;

  // Retrieve fixed dt
  float fixedDt = static_cast<float>(time.GetFixedDeltaTime());     // seconds
  double fixedDtMs = time.GetFixedDeltaTimeMs(); // milliseconds

  // Retrieve input instance
  InputSystem& inputSys = InputSystem::GetInstance();

  // For now just a loop forever
  while (true)
  {
    bool quit = false;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
        quit = true;
        break;
    }

    if (quit)
      break;

    time.Update();

    float dt = static_cast<float>(time.GetDeltaTime());
    double dtMs = time.GetDeltaTimeMs();

    // Update all engine systems
    inputSys.Update();

    accumulator += dtMs;

    while (accumulator >= fixedDtMs)
    {
      // physicsSys.Update(fixedDt);
      accumulator -= fixedDtMs;
    }

    inputSys.PostUpdate();
  }
}

void Application::InitializeCoreSystems()
{
  // Initialize the instance of time
  Time::Initialize();

  D3D11RenderAPI::Initialize();
  D3D11RenderWindowManager::Initialize();

  InputSystem::Initialize(::GetCapture());

  Physics::Initialize();
}

void Application::InitializeResources()
{
  HypeRenderer::LoadSystemShaders();
  HypeRenderer::CreateCommonResources();
  Scene::Initialize();
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

  HypeRenderer::Initialize();
}
