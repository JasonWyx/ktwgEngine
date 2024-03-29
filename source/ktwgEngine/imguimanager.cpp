#include "imguimanager.h"
#include "windowmanager.h"

ImGuiManager::ImGuiManager() : Singleton<ImGuiManager>{ }
  , m_Window{ nullptr }
  , m_Time{ 0.f }
  , m_MouseJustPressed{ /*false*/ }
  , m_MouseCursors{ /*nullptr*/ }
  , m_GlslVersionString{ }
  , m_FontTexture{ GL_NONE }
  , m_ShaderHandle{ GL_NONE }
  , m_VertHandle{ GL_NONE }
  , m_FragHandle{ GL_NONE }
  , m_AttribLocationTex{ GL_NONE }
  , m_AttribLocationProjMtx{ GL_NONE }
  , m_AttribLocationVtxPos{ GL_NONE }
  , m_AttribLocationVtxUV{ GL_NONE }
  , m_AttribLocationVtxColor{ GL_NONE }
  , m_VboHandle{ GL_NONE }
  , m_ElementsHandle{ GL_NONE }
{

}

ImGuiManager::~ImGuiManager()
{
}

// This late initialize is to allow other managers to register callbacks first without have this initialized
void ImGuiManager::LateInitialize()
{
  assert(WindowManager::GetInstance().IsInitialized());

  ImGui::CreateContext();
  InitGLFWStuff(WindowManager::GetInstance().GetWindow(), true);
  InitOpenGLStuff();
}

void ImGuiManager::Update()
{
  NewFrame();
  ImGui::NewFrame();

  if (ImGui::Begin("Debug Menu"))
  {
    if (ImGui::BeginTabBar("tabbar"))
    {
      for (Functor* callback : m_OnImGuiCallbacks)
      {
        if (ImGui::BeginTabItem(callback->GetTabHeader()))
        {
          callback->Invoke();
          ImGui::EndTabItem();
        }
      }
      ImGui::EndTabBar();
    }
  }
  ImGui::End(); 

  ImGui::EndFrame();
  ImGui::Render();
  RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::InitializeInternal()
{

}

void ImGuiManager::ShutdownInternal()
{
  // GLFW
  for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
  {
    glfwDestroyCursor(m_MouseCursors[cursor_n]);
    m_MouseCursors[cursor_n] = nullptr;
  }

  // OpenGL
  ImGuiManager::DestroyDeviceObjects();

  ImGui::DestroyContext();

}

bool ImGuiManager::InitGLFWStuff(GLFWwindow* window, bool install_callbacks)
{
  m_Window = window;
  m_Time = 0.0;

  // Setup back-end capabilities flags
  ImGuiIO& io = ImGui::GetIO();
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
  io.BackendPlatformName = "imgui_impl_glfw";

  // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
  io.KeyMap[ImGuiKey_Tab]         = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow]   = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow]  = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow]     = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow]   = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp]      = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown]    = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home]        = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End]         = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Insert]      = GLFW_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete]      = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace]   = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space]       = GLFW_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter]       = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape]      = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A]           = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C]           = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V]           = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X]           = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y]           = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z]           = GLFW_KEY_Z;

  io.SetClipboardTextFn = ImGuiManager::SetClipboardText;
  io.GetClipboardTextFn = ImGuiManager::GetClipboardText;
  io.ClipboardUserData = m_Window;
#if defined(_WIN32)
  io.ImeWindowHandle = (void*)glfwGetWin32Window(m_Window);
#endif

  m_MouseCursors[ImGuiMouseCursor_Arrow]      = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  m_MouseCursors[ImGuiMouseCursor_TextInput]  = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  m_MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);   // FIXME: GLFW doesn't have this.
  m_MouseCursors[ImGuiMouseCursor_ResizeNS]   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  m_MouseCursors[ImGuiMouseCursor_ResizeEW]   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  m_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
  m_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
  m_MouseCursors[ImGuiMouseCursor_Hand]       = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

  if (install_callbacks)
  {
    WindowManager::GetInstance().AddCharCallback(ImGuiManager::CharCallback);
    WindowManager::GetInstance().AddKeyCallback(ImGuiManager::KeyCallback);
    WindowManager::GetInstance().AddMouseButtonCallback(ImGuiManager::MouseButtonCallback);
    WindowManager::GetInstance().AddScrollCallback(ImGuiManager::ScrollCallback);
  }

  return true;
}

bool ImGuiManager::InitOpenGLStuff(const char* glsl_version)
{
  ImGuiIO& io = ImGui::GetIO();
  io.BackendRendererName = "imgui_impl_opengl3";

  // Store GLSL version string so we can refer to it later in case we recreate shaders. Note: GLSL version is NOT the same as GL version. Leave this to nullptr if unsure.
  if (glsl_version == nullptr)
    glsl_version = "#version 130";

  IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(m_GlslVersionString));
  strcpy(m_GlslVersionString, glsl_version);
  strcat(m_GlslVersionString, "\n");

  // Make a dummy GL call (we don't actually need the result)
  // IF YOU GET A CRASH HERE: it probably means that you haven't initialized the OpenGL function loader used by this code.
  // Desktop OpenGL 3/4 need a function loader. See the IMGUI_IMPL_OPENGL_LOADER_xxx explanation above.
  GLint current_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);

  return true;
}

void ImGuiManager::SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object)
{
  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

  // Setup viewport, orthographic projection matrix
  // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  float L = draw_data->DisplayPos.x;
  float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
  float T = draw_data->DisplayPos.y;
  float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
  const float ortho_projection[4][4] =
  {
      { 2.0f / (R - L),   0.0f,         0.0f,   0.0f },
      { 0.0f,         2.0f / (T - B),   0.0f,   0.0f },
      { 0.0f,         0.0f,        -1.0f,   0.0f },
      { (R + L) / (L - R),  (T + B) / (B - T),  0.0f,   1.0f },
  };
  glUseProgram(m_ShaderHandle);
  glUniform1i(m_AttribLocationTex, 0);
  glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
#ifdef GL_SAMPLER_BINDING
  glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif

  (void)vertex_array_object;
#ifndef IMGUI_IMPL_OPENGL_ES2
  glBindVertexArray(vertex_array_object);
#endif

  // Bind vertex/index buffers and setup attributes for ImDrawVert
  glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
  glEnableVertexAttribArray(m_AttribLocationVtxPos);
  glEnableVertexAttribArray(m_AttribLocationVtxUV);
  glEnableVertexAttribArray(m_AttribLocationVtxColor);
  glVertexAttribPointer(m_AttribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
  glVertexAttribPointer(m_AttribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
  glVertexAttribPointer(m_AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));
}

void ImGuiManager::RenderDrawData(ImDrawData* draw_data)
{
  // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
  int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
  int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
  if (fb_width <= 0 || fb_height <= 0)
    return;

  // Backup GL state
  GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)& last_active_texture);
  glActiveTexture(GL_TEXTURE0);
  GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
  GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
  GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
  GLint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);
#endif
#ifdef GL_POLYGON_MODE
  GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
  GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
  GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)& last_blend_src_rgb);
  GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)& last_blend_dst_rgb);
  GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)& last_blend_src_alpha);
  GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)& last_blend_dst_alpha);
  GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)& last_blend_equation_rgb);
  GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)& last_blend_equation_alpha);
  GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
  bool clip_origin_lower_left = true;
#if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
  GLenum last_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)& last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
  if (last_clip_origin == GL_UPPER_LEFT)
    clip_origin_lower_left = false;
#endif

  // Setup desired GL state
  // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
  // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
  GLuint vertex_array_object = 0;
#ifndef IMGUI_IMPL_OPENGL_ES2
  glGenVertexArrays(1, &vertex_array_object);
#endif
  ImGuiManager::SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

  // Will project scissor/clipping rectangles into framebuffer space
  ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
  ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

  // Render command lists
  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    size_t idx_buffer_offset = 0;

    // Upload vertex/index buffers
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
    {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback != nullptr)
      {
        // User callback, registered via ImDrawList::AddCallback()
        // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
          ImGuiManager::SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
        else
          pcmd->UserCallback(cmd_list, pcmd);
      }
      else
      {
        // Project scissor/clipping rectangles into framebuffer space
        ImVec4 clip_rect;
        clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
        clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
        clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
        clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

        if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
        {
          // Apply scissor/clipping rectangle
          if (clip_origin_lower_left)
            glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
          else
            glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)

        // Bind texture, Draw
          glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
          glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)idx_buffer_offset);
        }
      }
      idx_buffer_offset += pcmd->ElemCount * sizeof(ImDrawIdx);
    }
  }

  // Destroy the temporary VAO
#ifndef IMGUI_IMPL_OPENGL_ES2
  glDeleteVertexArrays(1, &vertex_array_object);
#endif

  // Restore modified GL state
  glUseProgram(last_program);
  glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
  glBindSampler(0, last_sampler);
#endif
  glActiveTexture(last_active_texture);
#ifndef IMGUI_IMPL_OPENGL_ES2
  glBindVertexArray(last_vertex_array_object);
#endif
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
  if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
  glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
  glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
  glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void ImGuiManager::UpdateMousePosAndButtons()
{
  // Update buttons
  ImGuiIO& io = ImGui::GetIO();
  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
  {
    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[i] = m_MouseJustPressed[i] || glfwGetMouseButton(m_Window, i) != 0;
    m_MouseJustPressed[i] = false;
  }

  // Update mouse position
  const ImVec2 mouse_pos_backup = io.MousePos;
  io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
  const bool focused = true; // Emscripten
#else
  const bool focused = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0;
#endif
  if (focused)
  {
    if (io.WantSetMousePos)
    {
      glfwSetCursorPos(m_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
    }
    else
    {
      double mouse_x, mouse_y;
      glfwGetCursorPos(m_Window, &mouse_x, &mouse_y);
      io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
    }
  }
}

void ImGuiManager::UpdateMouseCursor()
{
  ImGuiIO& io = ImGui::GetIO();
  if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    return;

  ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
  if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
  {
    // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
  else
  {
    // Show OS mouse cursor
    // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
    glfwSetCursor(m_Window, m_MouseCursors[imgui_cursor] ? m_MouseCursors[imgui_cursor] : m_MouseCursors[ImGuiMouseCursor_Arrow]);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void ImGuiManager::UpdateGamepads()
{
  ImGuiIO& io = ImGui::GetIO();
  memset(io.NavInputs, 0, sizeof(io.NavInputs));
  if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
    return;

  // Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
  int axes_count = 0, buttons_count = 0;
  const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
  const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
  MAP_BUTTON(ImGuiNavInput_Activate, 0);     // Cross / A
  MAP_BUTTON(ImGuiNavInput_Cancel, 1);     // Circle / B
  MAP_BUTTON(ImGuiNavInput_Menu, 2);     // Square / X
  MAP_BUTTON(ImGuiNavInput_Input, 3);     // Triangle / Y
  MAP_BUTTON(ImGuiNavInput_DpadLeft, 13);    // D-Pad Left
  MAP_BUTTON(ImGuiNavInput_DpadRight, 11);    // D-Pad Right
  MAP_BUTTON(ImGuiNavInput_DpadUp, 10);    // D-Pad Up
  MAP_BUTTON(ImGuiNavInput_DpadDown, 12);    // D-Pad Down
  MAP_BUTTON(ImGuiNavInput_FocusPrev, 4);     // L1 / LB
  MAP_BUTTON(ImGuiNavInput_FocusNext, 5);     // R1 / RB
  MAP_BUTTON(ImGuiNavInput_TweakSlow, 4);     // L1 / LB
  MAP_BUTTON(ImGuiNavInput_TweakFast, 5);     // R1 / RB
  MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
  MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
  MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
  MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);
#undef MAP_BUTTON
#undef MAP_ANALOG
  if (axes_count > 0 && buttons_count > 0)
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
  else
    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}

void ImGuiManager::NewFrame()
{
  // OpenGL
  if (!m_FontTexture)
    ImGuiManager::CreateDeviceObjects();

  //CreateFontsTexture();
  ImGuiIO& io = ImGui::GetIO();
  IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGuiManager::NewFrame().");

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(m_Window, &w, &h);
  glfwGetFramebufferSize(m_Window, &display_w, &display_h);
  io.DisplaySize = ImVec2((float)w, (float)h);
  if (w > 0 && h > 0)
    io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

  // Setup time step
  double current_time = glfwGetTime();
  io.DeltaTime = m_Time > 0.0 ? (float)(current_time - m_Time) : (float)(1.0f / 60.0f);
  m_Time = current_time;

  UpdateMousePosAndButtons();
  UpdateMouseCursor();

  // Update game controllers (if enabled and available)
  UpdateGamepads();
}

bool ImGuiManager::CreateFontsTexture()
{
  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

  // Upload texture to graphics system
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &m_FontTexture);
  glBindTexture(GL_TEXTURE_2D, m_FontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = (ImTextureID)(intptr_t)m_FontTexture;

  // Restore state
  glBindTexture(GL_TEXTURE_2D, last_texture);

  return true;
}

void ImGuiManager::DestroyFontsTexture()
{
  if (m_FontTexture)
  {
    ImGuiIO& io = ImGui::GetIO();
    glDeleteTextures(1, &m_FontTexture);
    io.Fonts->TexID = 0;
    m_FontTexture = 0;
  }
}

bool ImGuiManager::CheckShader(GLuint handle, const char* desc)
{
  GLint status = 0, lom_length = 0;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
  glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &lom_length);
  if ((GLboolean)status == GL_FALSE)
    fprintf(stderr, "ERROR: ImGuiManager::CreateDeviceObjects: failed to compile %s!\n", desc);
  if (lom_length > 0)
  {
    ImVector<char> buf;
    buf.resize((int)(lom_length + 1));
    glGetShaderInfoLog(handle, lom_length, nullptr, (GLchar*)buf.begin());
    fprintf(stderr, "%s\n", buf.begin());
  }
  return (GLboolean)status == GL_TRUE;
}

bool ImGuiManager::CheckProgram(GLuint handle, const char* desc)
{
  GLint status = 0, lom_length = 0;
  glGetProgramiv(handle, GL_LINK_STATUS, &status);
  glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &lom_length);
  if ((GLboolean)status == GL_FALSE)
    fprintf(stderr, "ERROR: ImGuiManager::CreateDeviceObjects: failed to link %s! (with GLSL '%s')\n", desc, m_GlslVersionString);
  if (lom_length > 0)
  {
    ImVector<char> buf;
    buf.resize((int)(lom_length + 1));
    glGetProgramInfoLog(handle, lom_length, nullptr, (GLchar*)buf.begin());
    fprintf(stderr, "%s\n", buf.begin());
  }
  return (GLboolean)status == GL_TRUE;
}

bool ImGuiManager::CreateDeviceObjects()
{
  // Backup GL state
  GLint last_texture, last_array_buffer;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
  GLint last_vertex_array;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#endif

  // Parse GLSL version string
  int glsl_version = 130;
  sscanf(m_GlslVersionString, "#version %d", &glsl_version);

  const GLchar* vertex_shader_glsl_120 =
    "uniform mat4 ProjMtx;\n"
    "attribute vec2 Position;\n"
    "attribute vec2 UV;\n"
    "attribute vec4 Color;\n"
    "varying vec2 Fram_UV;\n"
    "varying vec4 Fram_Color;\n"
    "void main()\n"
    "{\n"
    "    Fram_UV = UV;\n"
    "    Fram_Color = Color;\n"
    "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

  const GLchar* vertex_shader_glsl_130 =
    "uniform mat4 ProjMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Fram_UV;\n"
    "out vec4 Fram_Color;\n"
    "void main()\n"
    "{\n"
    "    Fram_UV = UV;\n"
    "    Fram_Color = Color;\n"
    "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

  const GLchar* vertex_shader_glsl_300_es =
    "precision mediump float;\n"
    "layout (location = 0) in vec2 Position;\n"
    "layout (location = 1) in vec2 UV;\n"
    "layout (location = 2) in vec4 Color;\n"
    "uniform mat4 ProjMtx;\n"
    "out vec2 Fram_UV;\n"
    "out vec4 Fram_Color;\n"
    "void main()\n"
    "{\n"
    "    Fram_UV = UV;\n"
    "    Fram_Color = Color;\n"
    "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

  const GLchar* vertex_shader_glsl_410_core =
    "layout (location = 0) in vec2 Position;\n"
    "layout (location = 1) in vec2 UV;\n"
    "layout (location = 2) in vec4 Color;\n"
    "uniform mat4 ProjMtx;\n"
    "out vec2 Fram_UV;\n"
    "out vec4 Fram_Color;\n"
    "void main()\n"
    "{\n"
    "    Fram_UV = UV;\n"
    "    Fram_Color = Color;\n"
    "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

  const GLchar* fragment_shader_glsl_120 =
    "#ifdef GL_ES\n"
    "    precision mediump float;\n"
    "#endif\n"
    "uniform sampler2D Texture;\n"
    "varying vec2 Fram_UV;\n"
    "varying vec4 Fram_Color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = Fram_Color * texture2D(Texture, Fram_UV.st);\n"
    "}\n";

  const GLchar* fragment_shader_glsl_130 =
    "uniform sampler2D Texture;\n"
    "in vec2 Fram_UV;\n"
    "in vec4 Fram_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "    Out_Color = Fram_Color * texture(Texture, Fram_UV.st);\n"
    "}\n";

  const GLchar* fragment_shader_glsl_300_es =
    "precision mediump float;\n"
    "uniform sampler2D Texture;\n"
    "in vec2 Fram_UV;\n"
    "in vec4 Fram_Color;\n"
    "layout (location = 0) out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "    Out_Color = Fram_Color * texture(Texture, Fram_UV.st);\n"
    "}\n";

  const GLchar* fragment_shader_glsl_410_core =
    "in vec2 Fram_UV;\n"
    "in vec4 Fram_Color;\n"
    "uniform sampler2D Texture;\n"
    "layout (location = 0) out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "    Out_Color = Fram_Color * texture(Texture, Fram_UV.st);\n"
    "}\n";

  // Select shaders matching our GLSL versions
  const GLchar* vertex_shader = nullptr;
  const GLchar* fragment_shader = nullptr;
  if (glsl_version < 130)
  {
    vertex_shader = vertex_shader_glsl_120;
    fragment_shader = fragment_shader_glsl_120;
  }
  else if (glsl_version >= 410)
  {
    vertex_shader = vertex_shader_glsl_410_core;
    fragment_shader = fragment_shader_glsl_410_core;
  }
  else if (glsl_version == 300)
  {
    vertex_shader = vertex_shader_glsl_300_es;
    fragment_shader = fragment_shader_glsl_300_es;
  }
  else
  {
    vertex_shader = vertex_shader_glsl_130;
    fragment_shader = fragment_shader_glsl_130;
  }

  // Create shaders
  const GLchar* vertex_shader_with_version[2] = { m_GlslVersionString, vertex_shader };
  m_VertHandle = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_VertHandle, 2, vertex_shader_with_version, nullptr);
  glCompileShader(m_VertHandle);
  CheckShader(m_VertHandle, "vertex shader");

  const GLchar* fragment_shader_with_version[2] = { m_GlslVersionString, fragment_shader };
  m_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_FragHandle, 2, fragment_shader_with_version, nullptr);
  glCompileShader(m_FragHandle);
  CheckShader(m_FragHandle, "fragment shader");

  m_ShaderHandle = glCreateProgram();
  glAttachShader(m_ShaderHandle, m_VertHandle);
  glAttachShader(m_ShaderHandle, m_FragHandle);
  glLinkProgram(m_ShaderHandle);
  CheckProgram(m_ShaderHandle, "shader program");

  m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
  m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");
  m_AttribLocationVtxPos = glGetAttribLocation(m_ShaderHandle, "Position");
  m_AttribLocationVtxUV = glGetAttribLocation(m_ShaderHandle, "UV");
  m_AttribLocationVtxColor = glGetAttribLocation(m_ShaderHandle, "Color");

  // Create buffers
  glGenBuffers(1, &m_VboHandle);
  glGenBuffers(1, &m_ElementsHandle);

  ImGuiManager::CreateFontsTexture();

  // Restore modified GL state
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
  glBindVertexArray(last_vertex_array);
#endif

  return true;
}

void ImGuiManager::DestroyDeviceObjects()
{
  if (m_VboHandle) glDeleteBuffers(1, &m_VboHandle);
  if (m_ElementsHandle) glDeleteBuffers(1, &m_ElementsHandle);
  m_VboHandle = m_ElementsHandle = 0;

  if (m_ShaderHandle && m_VertHandle) glDetachShader(m_ShaderHandle, m_VertHandle);
  if (m_VertHandle) glDeleteShader(m_VertHandle);
  m_VertHandle = 0;

  if (m_ShaderHandle && m_FragHandle) glDetachShader(m_ShaderHandle, m_FragHandle);
  if (m_FragHandle) glDeleteShader(m_FragHandle);
  m_FragHandle = 0;

  if (m_ShaderHandle) glDeleteProgram(m_ShaderHandle);
  m_ShaderHandle = 0;

  ImGuiManager::DestroyFontsTexture();
}

// GLFW  callback functions

void ImGuiManager::MouseButtonCallback(int button, int action, int mods)
{
  if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(GetInstance().m_MouseJustPressed))
    GetInstance().m_MouseJustPressed[button] = true;
}

void ImGuiManager::ScrollCallback(double xoffset, double yoffset)
{
  ImGuiIO& io = ImGui::GetIO();
  io.MouseWheelH += (float)xoffset;
  io.MouseWheel += (float)yoffset;
}

void ImGuiManager::KeyCallback(int key, int scancode, int action, int mods)
{
  ImGuiIO& io = ImGui::GetIO();
  if (action == GLFW_PRESS)
    io.KeysDown[key] = true;
  if (action == GLFW_RELEASE)
    io.KeysDown[key] = false;

  // Modifiers are not reliable across systems
  io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
  io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ImGuiManager::CharCallback(unsigned int c)
{
  ImGuiIO& io = ImGui::GetIO();
  if (c > 0 && c < 0x10000)
    io.AddInputCharacter((unsigned short)c);
}

// ImGui callback functions

const char* ImGuiManager::GetClipboardText(void* user_data)
{
  return glfwGetClipboardString((GLFWwindow*)user_data);
}

void ImGuiManager::SetClipboardText(void* user_data, const char* text)
{
  glfwSetClipboardString((GLFWwindow*)user_data, text);
}