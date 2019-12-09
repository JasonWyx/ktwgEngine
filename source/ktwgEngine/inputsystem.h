#pragma once

#include "singleton.h"
#include "win32input.h"
#include "win32_includes.h"

#define KEY_BUFFER_SIZE 256
#define MOUSE_BUFFER_SIZE 3

class InputSystem : public Singleton<InputSystem>
{
  friend class Win32Platform;

  int    m_Key[KEY_BUFFER_SIZE];                     // Keep track of state of key
  float  m_KeyDownDuration[KEY_BUFFER_SIZE];         // Keep track of duration of keydown
  float  m_KeyDownDurationPrev[KEY_BUFFER_SIZE];     // Keep track Previous duration of keydown

  bool   m_Mouse[MOUSE_BUFFER_SIZE];                 // Keep track of state of mouse
  bool   m_MouseDoubleClick[MOUSE_BUFFER_SIZE];      // Keep track of double click state of mouse
  float  m_MouseDownDuration[MOUSE_BUFFER_SIZE];     // Keep track of duration of mouse down
  float  m_MouseDownDurationPrev[MOUSE_BUFFER_SIZE]; // Keep track of previous duration of mouse down
  float  m_MouseClickTime[MOUSE_BUFFER_SIZE];        // Time of mouse click
  float  m_MouseDoubleClickTime;                   // Timing between two consecutive click to be considered double click
  double m_MouseWheel;                             // Keep track of vertical wheel scroll amount
  //Vec2   m_MouseScreenPos;                         // Keep track of position of the mouse
  //Vec2   m_MouseScreenPosInGameView;
  //Vec2   m_MouseDelta;                             // Keep track of absolute position of the mouse

  bool   m_LockCursor;                             // Set cursor to be confined at center every frame
  bool   m_ShowCursor;

  HWND   m_Hwnd;                                   // Handle to the client window

  //Vec2I mouseCenter_;

public:
  InputSystem(HWND hwnd);
  ~InputSystem();

  void InitializeInternal() override;
  void ShutdownInternal() override;

  /* Called every frame to update the input states */
  void Update();
  void PostUpdate();

  /* Query if key is KeyUpState */
  bool OnKeyUp(KTWGKeyCode code);

  /* Query if key is KeyDownState */
  bool OnKeyDown(KTWGKeyCode code);

  /* Query if key is KeyPressState */
  bool OnKeyPress(KTWGKeyCode code);

  /* Query if mouse button is release state*/
  bool OnMouseRelease(KTWGMouseCode code);

  /* Query if mouse button is down state*/
  bool OnMouseDown(KTWGMouseCode code);

  /* Query if mouse button is click state*/
  bool OnMouseClick(KTWGMouseCode code);

  /* Query if mouse button is double click state*/
  bool OnMouseDoubleClick(KTWGMouseCode code);

  /* Retrieves mouse position relative to window */
  //Vec2 GetMousePos() const;

  /* Retrieves mouse position relative to game view */
  //Vec2 GetMousePosInGameView() const;

  //Vec2 GetNormalizedMousePos() const;

  /* Retrieves mouse delta position */
  //Vec2 GetMouseDelta() const;

  /* Query if cursor is in lock state */
  bool GetLockCursor() const;

  /* Query if cursor is visible state */
  bool GetShowCursor() const;

  /* Sets the cursor to lock state */
  void SetLockCursor(bool lock);

  /* Show/Hide cursor*/
  void ShowCursor(bool show);

private:
  /* Map key down messages */
  void MapKeyDown(WPARAM wParam);

  /* Map Key up messages */
  void MapKeyUp(WPARAM wParam);

  /* Query if any mouse button is down */
  bool IsAnyMouseDown();
};
