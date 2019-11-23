/******************************************************************************/
/*!
\file       BPMath.h
\author     Low Jin Liang Aaron
\par        Project: BPEngine
\brief      This is the interface file for BPMath.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

#include <type_traits>

namespace Math
{

  template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
  inline T ConvertRadianToAngle(T radian)
  {
    static constexpr auto UNIT_DEGREES = 180.f / PI;
    return UNIT_DEGREES * radian;
  }

  template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
  inline T ConvertAngleToRadian(T angle)
  {
    static constexpr auto UNIT_RADIAN = PI / 180.0f;
    return UNIT_RADIAN * angle;
  }

  template <typename T>
  inline T Min(T a, T b)
  {
    return a < b ? a : b;
  }

  template <typename T>
  inline T Max(T a, T b)
  {
    return a > b ? a : b;
  }

  template <typename T>
  inline T Clamp(T min, T max, T value)
  {
    return Max(min, Min(value, max));
  }

  template <typename T>
  inline T Sign(T val)
  {
    if (val >= 0)
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }

  template<class T>
  inline T BPAbs(T a) { return a < 0 ? -a : a; }

  template <typename T>
  T Interpolate(const T& p0, const T& p1, float progression)
  {
    return p0 + progression * (p1 - p0);
  }

}

