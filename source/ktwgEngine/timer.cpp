#include "timer.h"

Timer::Timer()
  : m_StartTime{ m_Clock.now() }
{
}

void Timer::Reset()
{
  m_StartTime = m_Clock.now();
}

double Timer::GetElapsedTime()
{
  auto current_time = m_Clock.now();
  second_t elapsed = current_time - m_StartTime;

  return elapsed.count();
}

double Timer::GetElapsedTimeMs()
{
  auto current_time = m_Clock.now();
  second_t elapsed = current_time - m_StartTime;

  return std::chrono::duration_cast<millisecond_t>(elapsed).count();
}

double Timer::GetStartTime()
{
  return std::chrono::duration_cast<second_t>(m_StartTime.time_since_epoch()).count();
}

double Timer::GetStartTimeMs()
{
  return std::chrono::duration_cast<millisecond_t>(m_StartTime.time_since_epoch()).count();
}
