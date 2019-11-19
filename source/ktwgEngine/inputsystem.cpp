#include "inputsystem.h"
#include <cstring>

InputSystem::InputSystem(HWND hwnd)
  : m_MouseDoubleClickTime{ 500.0f }, m_MouseWheel{ 0.0f }, m_LockCursor{ false }, m_ShowCursor{ true }, m_Hwnd{ hwnd }
{
  for (auto i = 0; i < KEY_BUFFER_SIZE; ++i)
    m_Key[i] = 0;

  for (auto i = 0; i < MOUSE_BUFFER_SIZE; ++i)
    m_Mouse[i] = false;

  // Update cursor position
  POINT p;
  GetCursorPos(&p);
  ScreenToClient(m_Hwnd, &p);

  // mouseScreenPos_.x_ = static_cast<float>(p.x);
  // mouseScreenPos_.y_ = static_cast<float>(p.y);

  RECT rect;
  GetWindowRect(m_Hwnd, &rect);

  //mouseCenter_.x_ = (rect.right - rect.left) / 2;
  //mouseCenter_.y_ = (rect.bottom - rect.top) / 2;
}

InputSystem::~InputSystem()
{
}

void InputSystem::Update()
{
  float dt = 0.0f;
  //auto dt = static_cast<float>(GetTimeModule().GetDeltaTimeMs());

  //auto oldPos = mouseScreenPos_;

  // Update cursor position
  POINT p;
  GetCursorPos(&p);
  ScreenToClient(m_Hwnd, &p);

  //mouseScreenPos_.x_ = static_cast<float>(p.x);
  //mouseScreenPos_.y_ = static_cast<float>(p.y);

  //m_MouseDelta = m_MouseScreenPos_ - oldPos;

  // Keep track of previous frame key down duration
  std::memcpy(m_KeyDownDurationPrev, m_KeyDownDuration, sizeof(float)* KEY_BUFFER_SIZE);

  // Update current frame key down duration
  for (auto i = 0; i < KEY_BUFFER_SIZE; ++i)
    m_KeyDownDuration[i] = m_Key[i] ? (m_KeyDownDuration[i] < 0.0f ? 0.0f : m_KeyDownDuration[i] + dt) : -1.0f;

  // Keep track of previous frame mouse down duration
  std::memcpy(m_MouseDownDurationPrev, m_MouseDownDuration, sizeof(float)* MOUSE_BUFFER_SIZE);

  // Update current's frame mouse states
  for (auto i = 0; i < MOUSE_BUFFER_SIZE; ++i)
  {
    m_MouseDoubleClick[i] = false;
    m_MouseDownDuration[i] = m_Mouse[i] ? (m_MouseDownDuration[i] < 0.0f ? 0.0f : m_MouseDownDuration[i] + dt) : -1.0f;
    //if (OnMouseClick((KTWGMouseCode)i))
    //{
    //  // If 2nd click time and 1st click time is within threshold, double click confirmed
    //  if (static_cast<float>(GetTimeModule().GetTimeMs()) - mouseClickTime_[i] < mouseDoubleClickTime_)
    //  {
    //    mouseDoubleClick_[i] = true;
    //    mouseClickTime_[i] = -1000.0f; // Prevent third click to be captured
    //  }
    //  else // 1st click state. Keep track of 1st click time
    //    mouseClickTime_[i] = static_cast<float>(GetTimeModule().GetTimeMs());
    //}
  }
}

void InputSystem::PostUpdate()
{
  if (m_LockCursor)
  {
    RECT rect;
    GetWindowRect(m_Hwnd, &rect);
    auto width = rect.right - rect.left;
    auto height = rect.bottom - rect.top;

    /*if (mouseScreenPos_.x_ > 0.0f && mouseScreenPos_.x_ < width - 100 && mouseScreenPos_.y_ > 0.0f && mouseScreenPos_.y_ < height)
      return;

    POINT p;
    p.x = mouseCenter_.x_;
    p.y = mouseCenter_.y_;

    SetCursorPos(mouseCenter_.x_, mouseCenter_.y_);
    ScreenToClient(hWnd_, &p);

    mouseScreenPos_ = Vec2{ static_cast<float>(p.x), static_cast<float>(p.y) };*/
  }
}

bool InputSystem::OnKeyUp(KTWGKeyCode code)
{
  return m_KeyDownDurationPrev[code] >= 0.0f && !m_Key[code];
}

bool InputSystem::OnKeyDown(KTWGKeyCode code)
{
  return m_KeyDownDuration[code] >= 0.0f;
}

bool InputSystem::OnKeyPress(KTWGKeyCode code)
{
  return m_KeyDownDuration[code] == 0.0f;
}

bool InputSystem::OnMouseRelease(KTWGMouseCode code)
{
  return m_MouseDownDurationPrev[code] >= 0.0f && !m_Mouse[code];
}

bool InputSystem::OnMouseDown(KTWGMouseCode code)
{
  return m_MouseDownDuration[code] >= 0.0f;
}

bool InputSystem::OnMouseClick(KTWGMouseCode code)
{
  return m_MouseDownDuration[code] == 0.0f;
}

bool InputSystem::OnMouseDoubleClick(KTWGMouseCode code)
{
  return m_MouseDoubleClick[code];
}

bool InputSystem::GetLockCursor() const
{
  return m_LockCursor;
}

bool InputSystem::GetShowCursor() const
{
  return m_ShowCursor;
}

void InputSystem::SetLockCursor(bool lock)
{
  m_LockCursor = lock
}

void InputSystem::ShowCursor(bool show)
{
  // return value specifies the new display counter 
  // if bShow is true, increment counter by 1, else decrement 
  // shows cursor if display counter is greater than or equal to 0

  if (show)
  {
    m_ShowCursor = true;
    while (::ShowCursor(TRUE) < 0);
  }
  else
  {
    m_ShowCursor = false;
    while (::ShowCursor(FALSE) >= 0);
  }
}

void InputSystem::MapKeyDown(WPARAM wParam)
{
  m_Key[wParam] = 1;
}

void InputSystem::MapKeyUp(WPARAM wParam)
{
  m_Key[wParam] = 0;
}

bool InputSystem::IsAnyMouseDown()
{
  for (auto i = 0; i < MOUSE_BUFFER_SIZE; ++i)
  {
    if (m_Mouse[i])
      return true;
  }
  return false;
}
