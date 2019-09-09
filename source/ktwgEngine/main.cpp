#include <glm/glm.hpp>
#include <glew/glew.h>
#include <glfw/glfw3.h>
#include "imgui/imgui.h"
#include "singletons.h"

int main()
{
  WindowManager::Initialize();
  ImGuiManager::Initialize();

  // Main loop
  while (WindowManager::GetInstance().PollEvents())
  {
    glClearColor(.2f, .2f, .2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    WindowManager::GetInstance().SwapBuffers();
  }


  ImGuiManager::Shutdown();
  WindowManager::Shutdown();
  return 0;
}