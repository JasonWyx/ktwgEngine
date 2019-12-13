#include "application.h"
#include "connectionmanager.h"

#include <iostream>
#include <fstream>

#include "d3d11renderapi.h"
#include "d3d11renderwindow.h"
#include "d3d11renderwindowmanager.h"

#include "hyperenderer.h"
#include "hypegraphicsworld.h"

#include "gameplaymanager.h"
#include "time.h"
#include "inputsystem.h"
#include "physics.h"
#include "ktwgbehaviour.h"


#include "scene.h"

#include "connectionmanager.h"
#include "streammanager.h"

void Application::InitializeInternal()
{
  InitializeCoreSystems();
  InitializeResources();
  LateInitialize();
}

void Application::InitializeCoreSystems()
{
  // Initialize the instance of time
  Time::Initialize();

  ConnectionManager::Initialize();

  StreamManager::Initialize();

// #ifdef CLIENT
  D3D11RenderAPI::Initialize();
  D3D11RenderWindowManager::Initialize();
// #endif

  InputSystem::Initialize(::GetCapture());

  Physics::Initialize();

  KTWGBehaviour::Initialize();

  GameplayManager::Initialize();
}

void Application::InitializeResources()
{
// #ifdef CLIENT
  HypeRenderer::LoadSystemShaders();
  HypeRenderer::CreateCommonResources();
// #endif
}

void Application::LateInitialize()
{
#ifdef CLIENT
  ConnectionManager::GetInstance().ConnectToServer();
  StreamManager::GetInstance().GetGhostManager().GenerateGhostIDs();
  StreamManager::GetInstance().GetEventManager().RegisterEvents();
  HypeRenderer::Initialize();
#else
  StreamManager::GetInstance().GetGhostManager().GenerateGhostIDs();
  StreamManager::GetInstance().GetEventManager().RegisterEvents();
  HypeRenderer::Initialize();
//  HypeGraphicsWorld::Initialize();
#endif
  Scene::Initialize();
}

void Application::ShutdownInternal()
{
  Scene::Shutdown();

  KTWGBehaviour::Shutdown();

  Physics::Shutdown();

  InputSystem::Shutdown();

// #ifdef CLIENT
  D3D11RenderWindowManager::Shutdown();
  D3D11RenderAPI::Shutdown();
// #endif

  Time::Shutdown();

  StreamManager::Shutdown();

  ConnectionManager::Shutdown();
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
  Physics& physicsSys = Physics::GetInstance();

// #ifdef CLIENT
  HypeRenderer& renderSys = HypeRenderer::GetInstance();
// #endif

  KTWGBehaviour& behSys = KTWGBehaviour::GetInstance();

  ConnectionManager& conSys = ConnectionManager::GetInstance();

  StreamManager& streamSys = StreamManager::GetInstance();

  behSys.Init();
  behSys.Start();

#ifndef CLIENT
  // Connect to server
#endif // CLIENT

  float accumDt = 0.0f;

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
    if (accumDt >= 0.016f)
      inputSys.Update();

    accumulator += dtMs;

    while (accumulator >= fixedDtMs)
    {
      physicsSys.Update(fixedDt);
      accumulator -= fixedDtMs;
    }
    
    if (accumDt >= 0.016f)
    {

      
      streamSys.Update();
      conSys.Update();
      // conSys.Update();

      behSys.Update();

// #ifdef CLIENT
      renderSys.Update();
// #endif

      inputSys.PostUpdate();

      accumDt = 0.0f;
    }
    accumDt += dt;
  }
}
