#pragma once

#include <memory>
#include "singleton.h"

// Forward declarations
class Timer;

/* This class manages the time utility functions */
class Time : public Singleton<Time>
{
  template <typename T>
  using UniquePtr = std::unique_ptr<T>;

  UniquePtr<Timer> m_Timer; // The timer used to retrieve time

  double m_PrevFrameTime;     // Previous frame elapsed time for current frame in milliseconds
  double m_StartTime;         // Application start time in seconds
  double m_StartTimeMs;       // Application start time in milliseconds
  double m_FrameTime;         // Time elapsed since previous frame to current frame in seconds
  double m_FrameTimeMs;       // Time elapsed since previous frame to current frame in milliseconds
  double m_ElapsedTime;       // Time elapsed since application start time in seconds
  double m_ElapsedTimeMs;     // Time elapsed since application start time in milliseconds
  double m_FixedDeltaTime;    // Fixed delta time in seconds
  double m_FixedDeltaTimeMs;  // Fixed delta time in milliseconds
  double m_ScaledDeltaTime;   // Scaled delta time
  double m_ScaledDeltaTimeMs; // Scaled delta time in milliseconds

  double m_TimeScale; // Time scale

public:
  Time();
  ~Time();

  void InitializeInternal() override;
  void ShutdownInternal() override;

  /* Update the time, to be called once every update loop */
  void Update();

  /* Get start time of application in seconds */
  inline double GetStartTime() { return m_StartTime; }

  /* Get start time of application in milliseconds */
  inline double GetStartTimeMs() { return m_StartTimeMs; }

  /* Get time elapsed since start in seconds */
  double GetTime();

  /* Get time elapsed since start in milliseconds */
  double GetTimeMs();

  /* Get frame time of one frame in seconds */
  inline double GetDeltaTime() { return m_FrameTime; }

  /* Get frame time of one frame in milliseconds */
  inline double GetDeltaTimeMs() { return m_FrameTimeMs; }

  /* Get delta time of one frame in seconds */
  inline double GetScaledDeltaTime() { return m_ScaledDeltaTime; }

  /* Get delta time of one frame in milliseconds */
  inline double GetScaledDeltaTimeMs() { return m_ScaledDeltaTimeMs; }

  /* Get fixed delta time */
  inline double GetFixedDeltaTime() { return m_FixedDeltaTime; }

  /* Get fixed delta time in milliseconds */
  inline double GetFixedDeltaTimeMs() { return m_FixedDeltaTimeMs; }

  /* Get the time scale for the time */
  inline double GetTimeScale() const { return m_TimeScale; }

  /* Set the time scale for the time */
  void SetTimeScale(double scale);
};