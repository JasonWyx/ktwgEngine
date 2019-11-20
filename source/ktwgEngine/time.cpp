#include "Time.h"

#include "Time.h"
#include "Timer.h"

Time::Time()
  : m_Timer{ nullptr }, m_PrevFrameTime{ 0.0 }, m_StartTime{ 0.0 }, m_StartTimeMs{ 0.0 },
    m_FrameTime{ 0.0 }, m_FrameTimeMs{ 0.0 }, m_ElapsedTime{ 0.0 }, m_ElapsedTimeMs{ 0.0 },
    m_TimeScale{ 1.0 }
{
  m_Timer         = std::make_unique<Timer>();
  m_StartTime     = m_Timer->GetStartTime();
  m_StartTimeMs   = m_Timer->GetStartTimeMs();
  m_PrevFrameTime = m_Timer->GetElapsedTimeMs();

  // 60fps
  m_FixedDeltaTime   = 1.0 / 60.0;
  m_FixedDeltaTimeMs = m_FixedDeltaTime * 1000.0;
}

Time::~Time()
{
}

void Time::InitializeInternal()
{
}

void Time::ShutdownInternal()
{
}

void Time::Update()
{
  // Get current time
  double current_time = m_Timer->GetElapsedTimeMs();

  // Compute frame time
  m_FrameTimeMs = current_time - m_PrevFrameTime;
  m_FrameTime   = m_FrameTimeMs / 1000.0;

  m_ScaledDeltaTime   = m_TimeScale * m_FrameTime;
  m_ScaledDeltaTimeMs = m_TimeScale * m_FrameTimeMs;

  // Update elapsed time
  m_ElapsedTimeMs = current_time;
  m_ElapsedTime   = current_time / 1000.0;

  // Update previous frame time
  m_PrevFrameTime = current_time;
}

double Time::GetTime()
{
  return m_Timer->GetElapsedTime();
}

double Time::GetTimeMs()
{
  return m_Timer->GetElapsedTimeMs();
}

void Time::SetTimeScale(double scale)
{
  m_TimeScale = scale;
}
