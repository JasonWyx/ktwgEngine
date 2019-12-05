/******************************************************************************/
/*!
\file       Vector3.h
\author     Low Jin Liang Aaron
\par        Project: BPEngine
\brief      This is the interface file for Vector3.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

// -------- Includes ------------ //
#include <cmath>
#include <cfloat>
#include <cinttypes>

// ---------- Implementation Of Vector3 Class ------------ //
struct Vec3
{
  float x_;
  float y_;
  float z_;

  explicit Vec3(float x = 0.f, float y = 0.f, float z = 0.f)
    : x_{ x }, y_{ y }, z_{ z }
  {
  }

  bool operator<(const Vec3& rhs) const
  {
    if (x_ != rhs.x_)
      return x_ < rhs.x_;

    if (y_ != rhs.y_)
      return y_ < rhs.y_;

    return z_ < rhs.z_;
  }

  inline float* a() { return &x_; }
  inline const float* a() const { return &x_; }
  inline float& operator[](int index) { return (&x_)[index]; }
  inline float operator[](int index) const { return (&x_)[index]; }

  void SetZero()
  {
    x_ = 0.f;
    y_ = 0.f;
    z_ = 0.f;
  }

  void Set(float x, float y, float z)
  {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  Vec3& operator+=(const Vec3& rhs)
  {
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;

    return *this;
  }

  Vec3& operator-=(const Vec3& rhs)
  {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    z_ -= rhs.z_;

    return *this;
  }

  Vec3& operator*=(float rhs)
  {
    x_ *= rhs;
    y_ *= rhs;
    z_ *= rhs;

    return *this;
  }

  Vec3& operator/=(float rhs)
  {
    x_ /= rhs;
    y_ /= rhs;
    z_ /= rhs;

    return *this;
  }

  Vec3 Negate() const
  {
    return Vec3{ -x_, -y_, -z_ };
  }

  // Returns original length
  float Normalize()
  {
    float length = sqrtf(x_ * x_ + y_ * y_ + z_ * z_);

    if (length > FLT_EPSILON)
    {
      float multiplier = 1.f / length;
      x_ *= multiplier;
      y_ *= multiplier;
      z_ *= multiplier;
    }

    return length;
  }

  // ----------- Constant Vectors -------------- //
  static const Vec3 Vec3_Zero; // Zero Vector
  static const Vec3 Vec3_X;    // Right Vector
  static const Vec3 Vec3_Y;    // Up Vector
  static const Vec3 Vec3_Z;    // Inwards Vector
};

inline Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3{ lhs.x_ + rhs.x_, lhs.y_ + rhs.y_, lhs.z_ + rhs.z_ };
}

inline Vec3 operator+(const Vec3& lhs, float rhs)
{
  return Vec3{ lhs.x_ + rhs, lhs.y_ + rhs, lhs.z_ + rhs };
}

inline Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3{ lhs.x_ - rhs.x_, lhs.y_ - rhs.y_, lhs.z_ - rhs.z_ };
}

inline Vec3 operator*(const Vec3& lhs, float rhs)
{
  return Vec3{ lhs.x_ * rhs, lhs.y_ * rhs, lhs.z_ * rhs };
}

inline Vec3 operator/(const Vec3& lhs, float rhs)
{
  return Vec3{ lhs.x_ / rhs, lhs.y_ / rhs, lhs.z_ / rhs };
}

inline Vec3 operator/(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3{ lhs.x_ / rhs.x_, lhs.y_ / rhs.y_, lhs.z_ / rhs.z_ };
}

inline Vec3 operator*(float lhs, const Vec3& rhs)
{
  return rhs * lhs;
}

inline Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3{ lhs.x_ * rhs.x_, lhs.y_ * rhs.y_, lhs.z_ * rhs.z_ };
}

inline Vec3 operator/(float lhs, const Vec3& rhs)
{
  return rhs / lhs;
}

inline bool operator==(const Vec3& lhs, const Vec3& rhs)
{
  return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_ && lhs.z_ == rhs.z_;
}

inline float Dot(const Vec3& lhs, const Vec3& rhs)
{
  return lhs.x_ * rhs.x_ + lhs.y_ * rhs.y_ + lhs.z_ * rhs.z_;
}

inline float SqLength(const Vec3& vec)
{
  return vec.x_ * vec.x_ + vec.y_ * vec.y_ + vec.z_ * vec.z_;
}

inline float Length(const Vec3& vec)
{
  return sqrtf(SqLength(vec));
}

inline float SqDistance(const Vec3& lhs, const Vec3& rhs)
{
  return SqLength(rhs - lhs);
}

inline float Distance(const Vec3& lhs, const Vec3& rhs)
{
  return sqrtf(SqDistance(lhs, rhs));
}

inline Vec3 Cross(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3{ lhs.y_ * rhs.z_ - lhs.z_ * rhs.y_, lhs.z_ * rhs.x_ - lhs.x_ * rhs.z_, lhs.x_ * rhs.y_ - lhs.y_ * rhs.x_ };
}

inline Vec3 Normalize(const Vec3& rhs)
{
  Vec3 result;

  float length = sqrtf(rhs.x_ * rhs.x_ + rhs.y_ * rhs.y_ + rhs.z_ * rhs.z_);

  if (length > FLT_EPSILON)
  {
    float multiplier = 1.f / length;
    result.x_ = rhs.x_ * multiplier;
    result.y_ = rhs.y_ * multiplier;
    result.z_ = rhs.z_ * multiplier;
  }

  return result;
}

// Returns the Determinant of a matrix formed by the column vectors u, v and w
inline float Det(const Vec3& u, const Vec3& v, const Vec3& w)
{
  return Dot(u, Cross(v, w));
}

// Returns the area of the triangle bounded by the three points
inline float Area(const Vec3& a, const Vec3& b, const Vec3& c)
{
  return 0.5f * Length(Cross(b - a, c - a));
}

inline float SqArea(const Vec3& a, const Vec3& b, const Vec3& c)
{
  return 0.25f * SqLength(Cross(b - a, c - a));
}

inline float Volume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d)
{
  float vol = Det(b - a, c - a, d - a);

  vol /= 6.0f;

  // Force a positive volume
  return vol < 0 ? -vol : vol;
}

inline float SqVolume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d)
{
  float vol = Volume(a, b, c, d);

  return vol * vol;
}

inline Vec3 MinVec(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3(lhs.x_ < rhs.x_ ? lhs.x_ : rhs.x_, lhs.y_ < rhs.y_ ? lhs.y_ : rhs.y_, lhs.z_ < rhs.z_ ? lhs.z_ : rhs.z_);
}

inline Vec3 MaxVec(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3(lhs.x_ > rhs.x_ ? lhs.x_ : rhs.x_, lhs.y_ > rhs.y_ ? lhs.y_ : rhs.y_, lhs.z_ > rhs.z_ ? lhs.z_ : rhs.z_);
}

inline Vec3 Multiply(const Vec3& lhs, const Vec3& rhs)
{
  return Vec3(lhs.x_ * rhs.x_, lhs.y_ * rhs.y_, lhs.z_ * rhs.z_);
}

inline Vec3 Absolute(const Vec3& v)
{
  return Vec3{ abs(v.x_), abs(v.y_), abs(v.z_) };
}

inline Vec3 Perpendicular(const Vec3& vec)
{
  // If (s, s, s) is a unit vector, s * s + s * s + s * s = 1, s = sqrt(1/3) = 0.57735..
  // At least one component is greater than or equal to 0.57735
  // If the x element exists, it is not on the y-z plane
  if (fabsf(vec.x_) >= 0.57735027f)
  {
    return Vec3{ vec.y_, -vec.x_, 0.f };
  }

  return Vec3{ 0.f, vec.z_, -vec.y_ };
}

inline float GetAngle(const Vec3& lhs, const Vec3& rhs)
{
  auto cross = Cross(lhs, rhs);
  auto a = Length(cross) / (Length(lhs) * Length(rhs));
  a = a > 1.f ? 1.f : (a < -1.f ? -1.f : a);
  return asinf(a);
}

inline void ComputeBasis(const Vec3& a, Vec3* b, Vec3* c)
{
  // Suppose vector a has all equal components and is a unit vector: a = (s, s, s)
  // Then 3*s*s = 1, s = sqrt(1/3) = 0.57735027. This means that at least one component of a
  // unit vector must be greater or equal to 0.57735027.

  if (abs(a.x_) >= 0.57735027f)
    b->Set(a.y_, -a.x_, 0.0f);
  else
    b->Set(0.0f, a.z_, -a.y_);

  b->Normalize();
  *c = Cross(a, *b);
}

const Vec3 VECTOR_X{ 1, 0, 0 };
const Vec3 VECTOR_Y{ 0, 1, 0 };
const Vec3 VECTOR_Z{ 0, 0, 1 };
