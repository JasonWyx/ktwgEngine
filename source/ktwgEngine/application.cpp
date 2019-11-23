#include "application.h"

#include <iostream>
#include <fstream>

#include "d3d11renderapi.h"
#include "d3d11renderwindow.h"
#include "d3d11renderwindowmanager.h"

#include "hyperenderer.h"

#include "time.h"
#include "inputsystem.h"
#include "physics.h"
#include "ktwgbehaviour.h"

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

  KTWGBehaviour::Shutdown();

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

  HypeRenderer& renderSys = HypeRenderer::GetInstance();
  KTWGBehaviour& behSys = KTWGBehaviour::GetInstance();

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
      behSys.Update();
      accumulator -= fixedDtMs;
    }

    renderSys.Update();
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

  KTWGBehaviour::Initialize();
}

void Application::InitializeResources()
{
  HypeRenderer::LoadSystemShaders();
  HypeRenderer::CreateCommonResources();
  Scene::Initialize();
}

void Application::LateInitialize()
{
  HypeRenderer::Initialize();
}
