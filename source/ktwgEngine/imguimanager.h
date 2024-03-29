#pragma once
#include "singleton.h"
#include "imgui/imgui.h"
// GLFW
#include <glew/glew.h>
#include <glfw/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>   // for glfwGetWin32Window
#endif
#define GLFW_HAS_WINDOW_TOPMOST     (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED     (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI    (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN             (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#include <stdio.h>
#include <stdint.h>
#include <vector>

class ImGuiManager : public Singleton<ImGuiManager>
{
public:

  ImGuiManager();
  ~ImGuiManager();

  void LateInitialize();
  void Update();

  template<typename T>
  void RegisterCallback(T* target, void(T::* function)(), const char* tabHeader)
  {
    m_OnImGuiCallbacks.push_back(new CallbackFunctor<T>(target, function, tabHeader));
  }

private:

  // Singleton internals
  virtual void InitializeInternal() override;
  virtual void ShutdownInternal() override;

  bool InitGLFWStuff(GLFWwindow* window, bool install_callbacks);
  bool InitOpenGLStuff(const char* glsl_version = NULL);
  void SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object);
  void RenderDrawData(ImDrawData* draw_data);
  void UpdateMousePosAndButtons();
  void UpdateMouseCursor();
  void UpdateGamepads();
  void NewFrame();

  // OpenGL helper functions
  bool CreateFontsTexture();
  void DestroyFontsTexture();
  bool CheckShader(GLuint handle, const char* desc);
  bool CheckProgram(GLuint handle, const char* desc);
  bool CreateDeviceObjects();
  void DestroyDeviceObjects();

  // GLFW callback functions
  static void MouseButtonCallback(int button, int action, int mods);
  static void ScrollCallback(double xoffset, double yoffset);
  static void KeyCallback(int key, int scancode, int action, int mods);
  static void CharCallback(unsigned int c);
  // ImGui callback functions
  static const char* GetClipboardText(void* user_data);
  static void SetClipboardText(void* user_data, const char* text);

  GLFWwindow* m_Window;
  double m_Time;
  bool m_MouseJustPressed[5];
  GLFWcursor* m_MouseCursors[ImGuiMouseCursor_COUNT];

  // OpenGL Data
  GLchar m_GlslVersionString[32];
  GLuint m_FontTexture;
  GLuint m_ShaderHandle;
  GLuint m_VertHandle;
  GLuint m_FragHandle;
  GLint m_AttribLocationTex;
  GLint m_AttribLocationProjMtx;
  GLint m_AttribLocationVtxPos;
  GLint m_AttribLocationVtxUV;
  GLint m_AttribLocationVtxColor;
  GLuint m_VboHandle;
  GLuint m_ElementsHandle;

  // Functor object helpers to invoke member function with bound target
  class Functor
  {
  public:
    virtual void Invoke() = 0;
    virtual const char* GetTabHeader() = 0;
  };

  template<typename T>
  class CallbackFunctor : public Functor
  {
  public:

    CallbackFunctor(T* target, void(T::* functionPtr)(), const char* tabHeader)
      : m_Target(target)
      , m_FunctionPtr(functionPtr)
      , m_TabHeader(tabHeader) { }

    T* m_Target;
    void (T::* m_FunctionPtr)();
    const char* m_TabHeader;

    virtual void Invoke() override
    {
      (m_Target->*m_FunctionPtr)();
    }

    // This tab header should only be a string literal
    virtual const char* GetTabHeader() override
    {
      return m_TabHeader;
    }
  };

  std::vector<Functor*> m_OnImGuiCallbacks;

};