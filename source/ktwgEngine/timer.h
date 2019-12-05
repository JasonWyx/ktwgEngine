#pragma once

#include <chrono>

/* This is a wrapper class for querying time */
class Timer
{
  // Using definitions
  using clock_t       = std::chrono::high_resolution_clock;
  using second_t      = std::chrono::duration<double>;
  using millisecond_t = std::chrono::duration<double, std::milli>;

  clock_t m_Clock;                              // The clock used to query for time
  std::chrono::time_point<clock_t> m_StartTime; // System start time
public:
  Timer();

  /* Reset the start time of the clock */
  void Reset();

  /* Returns the elapsed time since the start time in Seconds */
  double GetElapsedTime();

  /* Returns the elapsed time since the start time in Seconds */
  double GetElapsedTimeMs();

  /* Returns the start time of the time in Seconds */
  double GetStartTime();

  /* Returns the start time of the timer in Milliseconds */
  double GetStartTimeMs();

};