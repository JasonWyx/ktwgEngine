#pragma once
#include "singleton.h"
#include <glew/glew.h>
#include <glfw/glfw3.h>
#include <vector>

// Simple wrapper class around GLFW
class WindowManager : public Singleton<WindowManager>
{
public:

  WindowManager();
  ~WindowManager();

  bool PollEvents();
  void SwapBuffers();

  GLFWwindow* GetWindow() const { return m_Window; }
  int GetWindowWidth() const { return m_WindowWidth; }
  int GetWindowHeight() const { return m_WindowHeight; }
  void SetWindowSize(int width, int height);

private:

  // Inherited via Singleton
  virtual void InitializeInternal() override;
  virtual void ShutdownInternal() override;

  void OnImGui();

  GLFWwindow* m_Window;
  int m_WindowWidth;
  int m_WindowHeight;


  // Windows callback funtions
private:

  // Helper meta functions to get rid of GLFWwindow parameter
  template<typename Func> struct WindowsCallbackFunction { };
  template<typename ...Args> struct WindowsCallbackFunction<void(*)(Args...)> { static void function(Args... args); };
  template<typename ...Args> struct WindowsCallbackFunction<void(*)(GLFWwindow*, Args...)> { static void function(Args... args); };

  using WindowsCharCallbackFn         = decltype(&WindowsCallbackFunction<GLFWcharfun>::function);
  using WindowsCursorPosCallbackFn    = decltype(&WindowsCallbackFunction<GLFWcursorposfun>::function);
  using WindowsFileDropCallbackFn     = decltype(&WindowsCallbackFunction<GLFWdropfun>::function);
  using WindowsKeyCallbackFn          = decltype(&WindowsCallbackFunction<GLFWkeyfun>::function);
  using WindowsMouseButtonCallbackFn  = decltype(&WindowsCallbackFunction<GLFWmousebuttonfun>::function);
  using WindowsScrollCallbackFn       = decltype(&WindowsCallbackFunction<GLFWscrollfun>::function);

  // Actual GLFW callbacks
  static void WindowsCharCallbackFnInternal        (GLFWwindow* window, unsigned int codepoint);
  static void WindowsCursorPosCallbackFnInternal   (GLFWwindow* window, double xpos, double ypos);
  static void WindowsFileDropCallbackFnInternal    (GLFWwindow* window, int count, const char** paths);
  static void WindowsKeyCallbackFnInternal         (GLFWwindow* window, int key, int scancode, int action, int mods);
  static void WindowsMouseButtonCallbackFnInternal (GLFWwindow* window, int button, int action, int mods);
  static void WindowsScrollCallbackFnInternal      (GLFWwindow* window, double xoffset, double yoffset);

  std::vector<WindowsCharCallbackFn       > m_WindowsCharCallbackFns;
  std::vector<WindowsCursorPosCallbackFn  > m_WindowsCursorPosCallbackFns;
  std::vector<WindowsFileDropCallbackFn   > m_WindowsFileDropCallbackFns;
  std::vector<WindowsKeyCallbackFn        > m_WindowsKeyCallbackFns;
  std::vector<WindowsMouseButtonCallbackFn> m_WindowsMouseButtonCallbackFns;
  std::vector<WindowsScrollCallbackFn     > m_WindowsScrollCallbackFns;

public:

  // Add function pointers 
  void AddCharCallback(WindowsCharCallbackFn func)                { m_WindowsCharCallbackFns.push_back(func); }
  void AddCursorPosCallback(WindowsCursorPosCallbackFn func)      { m_WindowsCursorPosCallbackFns.push_back(func); }
  void AddFileDropCallback(WindowsFileDropCallbackFn func)        { m_WindowsFileDropCallbackFns.push_back(func); }
  void AddKeyCallback(WindowsKeyCallbackFn func)                  { m_WindowsKeyCallbackFns.push_back(func); }
  void AddMouseButtonCallback(WindowsMouseButtonCallbackFn func)  { m_WindowsMouseButtonCallbackFns.push_back(func); }
  void AddScrollCallback(WindowsScrollCallbackFn func)            { m_WindowsScrollCallbackFns.push_back(func); }

};