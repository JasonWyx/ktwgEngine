#include "windowmanager.h"
#include "singletons.h"

WindowManager::WindowManager() : Singleton<WindowManager>{ }
  , m_Window{ nullptr }
{

}

WindowManager::~WindowManager()
{
  
}

bool WindowManager::PollEvents()
{
  glfwPollEvents();
  return glfwWindowShouldClose(m_Window) == 0;
}

void WindowManager::SwapBuffers()
{
  glfwSwapBuffers(m_Window);
}

void WindowManager::SetWindowSize(int width, int height)
{
  m_WindowWidth = width;
  m_WindowHeight = height;

  if (m_Window != nullptr)
  {
    glfwSetWindowSize(m_Window, width, height);
  }
}

void WindowManager::InitializeInternal()
{
  assert(glfwInit());

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  m_Window = glfwCreateWindow(1024, 768, "ktwgEngine", nullptr, nullptr);

  glfwSetCharCallback(m_Window, &WindowsCharCallbackFnInternal);
  glfwSetCursorPosCallback(m_Window, &WindowsCursorPosCallbackFnInternal);
  glfwSetDropCallback(m_Window, &WindowsFileDropCallbackFnInternal);
  glfwSetKeyCallback(m_Window, &WindowsKeyCallbackFnInternal);
  glfwSetMouseButtonCallback(m_Window, &WindowsMouseButtonCallbackFnInternal);
  glfwSetScrollCallback(m_Window, &WindowsScrollCallbackFnInternal);

  // Move window to center of screen
  //glfwSetWindowPos(m_Window, (width / 2) - (desiredWindowWidth / 2), (height / 2) - (desiredWindowHeight / 2));
  glfwMakeContextCurrent(m_Window);
  glfwShowWindow(m_Window);
  glfwSwapInterval(0);

  assert(glewInit() == GLEW_OK);

  ImGuiManager::GetInstance().RegisterCallback(this, &WindowManager::OnImGui, "Window Manager");
}

void WindowManager::ShutdownInternal()
{
  glfwTerminate();
}

void WindowManager::OnImGui()
{

}

void WindowManager::WindowsCharCallbackFnInternal(GLFWwindow* window, unsigned int codepoint)
{
  for (WindowsCharCallbackFn callback : WindowManager::GetInstance().m_WindowsCharCallbackFns)
  {
    callback(codepoint);
  }
}

void WindowManager::WindowsCursorPosCallbackFnInternal(GLFWwindow* window, double xpos, double ypos)
{
  for (WindowsCursorPosCallbackFn callback : WindowManager::GetInstance().m_WindowsCursorPosCallbackFns)
  {
    callback(xpos, ypos);
  }
}

void WindowManager::WindowsFileDropCallbackFnInternal(GLFWwindow* window, int count, const char** paths)
{
  for (WindowsFileDropCallbackFn callback : WindowManager::GetInstance().m_WindowsFileDropCallbackFns)
  {
    callback(count, paths);
  }
}

void WindowManager::WindowsKeyCallbackFnInternal(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  for (WindowsKeyCallbackFn callback : WindowManager::GetInstance().m_WindowsKeyCallbackFns)
  {
    callback(key, scancode, action, mods);
  }
}

void WindowManager::WindowsMouseButtonCallbackFnInternal(GLFWwindow* window, int button, int action, int mods)
{
  for (WindowsMouseButtonCallbackFn callback : WindowManager::GetInstance().m_WindowsMouseButtonCallbackFns)
  {
    callback(button, action, mods);
  }
}

void WindowManager::WindowsScrollCallbackFnInternal(GLFWwindow* window, double xoffset, double yoffset)
{
  for (WindowsScrollCallbackFn callback : WindowManager::GetInstance().m_WindowsScrollCallbackFns)
  {
    callback(xoffset, yoffset);
  }
}
