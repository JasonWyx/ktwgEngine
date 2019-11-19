#include <glm/glm.hpp>
#include <glew/glew.h>
#include <glfw/glfw3.h>
#include "imgui/imgui.h"
#include "singletons.h"

int main()
{
  ImGuiManager::Initialize();
  WindowManager::Initialize();
  //ConnectionManager::Initialize();

  ImGuiManager::GetInstance().LateInitialize();

  Scene::Initialize();

  // Main loop
  while (WindowManager::GetInstance().PollEvents())
  {
	  //ConnectionManager::GetInstance().Update();
    glClearColor(.2f, .2f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGuiManager::GetInstance().Update();
    WindowManager::GetInstance().SwapBuffers();
  }

  ConnectionManager::Shutdown();
  ImGuiManager::Shutdown();
  WindowManager::Shutdown();
  return 0;
}