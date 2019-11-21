/******************************************************************************/
/*!
\file       Quaternion.h
\author     Low Jin Liang Aaron
\par        Project: BPEngine
\brief      This is the interface file for Quaternion.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once
#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"
#include "math.h"
#include <algorithm>

constexpr auto FLT_EPS = 1.0e-5;

/* Forward declaration */
struct Quaternion;

float      Dot(const Quaternion& lhs, const Quaternion& rhs);
Quaternion Add(const Quaternion& lhs, const Quaternion& rhs);
Quaternion Subtract(const Quaternion& lhs, const Quaternion& rhs);
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
Quaternion Scale(const Quaternion& lhs, float k);
Quaternion operator+                   (const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator-                   (const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator*                   (const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator*                   (const Quaternion& lhs, float k);
Quaternion operator*                   (float k, const Quaternion& lhs);
Quaternion Derivative(const Quaternion& orientation, const Vec3& velocity);
Quaternion ConvertAxisAngleToQuaternion(Vec3 axis, float radian);
Quaternion ConvertEulerToQuaternion(const Vec3& euler);
Quaternion ConvertMat33ToQuaternion(const Matrix3& mat);
Matrix3    ConvertQuaternionToMat33(const Quaternion& quat);
void       ConvertQuaternionToAxisAngle(Quaternion quat, Vec3& outVec, float& outRadian);

/* Represents a quaternion object. Can be used to apply rotational transforms to prevent gimbal lock. */
struct Quaternion
{
  float x_; // axis of rotation x
  float y_; // axis of rotation y
  float z_; // axis of rotation w
  float w_; // amount of rotation w

  /* Non-default constructors with default values. Constructs a quaternion by providing all 4 values */
  inline Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) noexcept
    : x_{ x }, y_{ y }, z_{ z }, w_{ w }
  {
  }

  /* Constructs a Quaternion using a vec3 and float */
  inline Quaternion(const Vec3& v, float w = 1.0f)
    : x_{ v.x_ }, y_{ v.y_ }, z_{ v.z_ }, w_{ w }
  {
  }

  /* Copy constructor */
  Quaternion(const Quaternion& rhs) = default;

  /* Move constructor */
  Quaternion(Quaternion&& rhs) = default;

  /* Returns an identity quaternion */
  inline Quaternion Identity()
  {
    return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
  }

  /* Returns the normalized squared result of quaternion */
  inline float NormSq() const
  {
    return (x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
  }

  /* Returns the magnitude of the quaternion */
  inline float Magnitude()
  {
    return sqrt(NormSq());
  }

  /* Normalize the quaternion into unit quaternion */
  inline void Normalize()
  {
    auto denom = this->Magnitude();

    // Prevent division by 0 or a number that is close to zero error
    if (denom > FLT_EPSILON)
    {
      auto norm_value = 1.0f / denom;

      this->Scale(norm_value);
    }
  }

  /* Returns conjugation of quaternion */
  inline Quaternion Conjugate() const
  {
    return Quaternion{ -x_, -y_, -z_, w_ };
  }

  /* Returns the inverse of quaternion */
  inline Quaternion Inverse() const
  {
    return Conjugate().Scale(1 / NormSq());
  }

  /* Usage : q1.Add(q2) ==  (q1 = q1 + q2) */
  inline Quaternion& Add(const Quaternion& rhs)
  {
    return (*this += rhs);
  }

  /* Usage : q1.Subtract(q2) ==  (q1 = q1 - q2) */
  inline Quaternion& Subtract(const Quaternion& rhs)
  {
    return (*this -= rhs);
  }

  /* Usage : q1.Multiply(q2) == (q1 = q1 * q2) */
  inline Quaternion& Multiply(const Quaternion& rhs)
  {
    return (*this *= rhs);
  }

  /* Usage : q1.Scale(k) */
  inline Quaternion& Scale(float k)
  {
    return (*this *= k);
  }

  /* Set quaternion to identity quaternion */
  inline void SetIdentity()
  {
    x_ = 0.0f;
    y_ = 0.0f;
    z_ = 0.0f;
    w_ = 1.0f;
  }

  /* Convert quaternion to vector and angle form */
  inline void ConvertToAxisAngle(Vec3& outVec, float& angle)
  {
    this->Normalize();

    auto sinAngle = sqrtf(1.0f - this->w_ * this->w_);

    if (fabsf(sinAngle) < FLT_EPSILON)
      sinAngle = 1.0f;

    outVec = Vec3{ this->x_ / sinAngle, this->y_ / sinAngle, this->z_ / sinAngle };

    angle = Math::ConvertRadianToAngle(acos(this->w_) * 2.0f);
  }

  /* Rotate vector by this quaternion */
  inline Quaternion Rotate(Vec3& v) const
  {
    // auto m = ConvertQuaternionToMat33(*this);
    // auto r2 = ::Multiply(m, v);

    /* Create pure quaternion with given vector */
    Quaternion p{ v, 0 };

    /* Apply quaternion rotation */
    Quaternion result = (*this) * p * this->Inverse();

    v.x_ = result.x_;
    v.y_ = result.y_;
    v.z_ = result.z_;

    return result;
  }

  /* Convert this quaternion to Matrix3 */
  inline Matrix3 ToMat33() const
  {
    Matrix3 result;
    float* matrix = result.m_[0];

    auto xx = x_ * x_;
    auto yy = y_ * y_;
    auto zz = z_ * z_;
    auto xy = x_ * y_;
    auto xz = x_ * z_;
    auto yz = y_ * z_;
    auto wx = w_ * x_;
    auto wy = w_ * y_;
    auto wz = w_ * z_;

    matrix[0] = 1.0f - 2.0f * (yy + zz);
    matrix[1] = 2.0f * (xy + wz);
    matrix[2] = 2.0f * (xz - wy);

    matrix[3] = 2.0f * (xy - wz);
    matrix[4] = 1.0f - 2.0f * (xx + zz);
    matrix[5] = 2.0f * (yz + wx);

    matrix[6] = 2.0f * (xz + wy);
    matrix[7] = 2.0f * (yz - wx);
    matrix[8] = 1.0f - 2.0f * (xx + yy);

    return result;
  }

  inline void Integrate(const Vec3& dv, float dt)
  {
    auto dvdt = dv * dt;
    Quaternion q{ dvdt.x_, dvdt.y_, dvdt.z_, 0.0f };

    q *= *this;

    x_ += q.x_ * 0.5f;
    y_ += q.y_ * 0.5f;
    z_ += q.z_ * 0.5f;
    w_ += q.w_ * 0.5f;

    this->Normalize();
  }

  /* Usage : (q1 += q2) == (q1 = q1 + q2) */
  inline Quaternion& operator+=(const Quaternion& rhs)
  {
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;
    w_ += rhs.w_;

    return *this;
  }

  /* Usage : (q1 -= q2) == (q1 = q1 - q2) */
  inline Quaternion& operator-=(const Quaternion& rhs)
  {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    z_ -= rhs.z_;
    w_ -= rhs.w_;

    return *this;
  }

  /* Usage : (q1 *= q2) == (q1 = q1 * q2) */
  inline Quaternion& operator*=(const Quaternion& rhs)
  {
    auto x = w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_;
    auto y = w_ * rhs.y_ - x_ * rhs.z_ + y_ * rhs.w_ + z_ * rhs.x_;
    auto z = w_ * rhs.z_ + x_ * rhs.y_ - y_ * rhs.x_ + z_ * rhs.w_;
    auto w = w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_;

    x_ = x;
    y_ = y;
    z_ = z;
    w_ = w;

    return *this;
  }

  /* Usage : (q1 *= k) == (q1 = q1 * k) */
  inline Quaternion& operator*=(float k)
  {
    x_ *= k;
    y_ *= k;
    z_ *= k;
    w_ *= k;

    return *this;
  }

  /* Returns true if q1 == q2 */
  inline bool operator==(const Quaternion& rhs) const
  {
    return (x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_ && w_ == rhs.w_);
  }

  /* Returns true if q1 != q2 */
  inline bool operator!=(const Quaternion& rhs) const
  {
    return !(*this == rhs);
  }

  /* copy assignment operator : q1 = q2; */
  Quaternion& operator=(const Quaternion& rhs)
  {
    x_ = rhs.x_;
    y_ = rhs.y_;
    z_ = rhs.z_;
    w_ = rhs.w_;

    return *this;
  }

  /* move assignment operator : q1 = q2; */
  Quaternion& operator=(Quaternion&& rhs) noexcept
  {
    x_ = std::move(rhs.x_);
    y_ = std::move(rhs.y_);
    z_ = std::move(rhs.z_);
    w_ = std::move(rhs.w_);

    return *this;
  }
};

/* Usage : q3 = lhs + rhs */
inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
  return Quaternion{ lhs.x_ + rhs.x_,
                     lhs.y_ + rhs.y_,
                     lhs.z_ + rhs.z_,
                     lhs.w_ + rhs.w_ };
}

/* Usage : q3 = lhs - rhs */
inline Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
  return Quaternion{ lhs.x_ - rhs.x_,
                     lhs.y_ - rhs.y_,
                     lhs.z_ - rhs.z_,
                     lhs.w_ - rhs.w_ };
}

/* Usage : q3 = lhs * rhs */
inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
  return Quaternion{ lhs.w_ * rhs.x_ + lhs.x_ * rhs.w_ + lhs.y_ * rhs.z_ - lhs.z_ * rhs.y_,
                      lhs.w_ * rhs.y_ - lhs.x_ * rhs.z_ + lhs.y_ * rhs.w_ + lhs.z_ * rhs.x_,
                      lhs.w_ * rhs.z_ + lhs.x_ * rhs.y_ - lhs.y_ * rhs.x_ + lhs.z_ * rhs.w_,
                      lhs.w_ * rhs.w_ - lhs.x_ * rhs.x_ - lhs.y_ * rhs.y_ - lhs.z_ * rhs.z_ };
}

/* Usage : q3 = lhs * k */
inline Quaternion operator*(const Quaternion& lhs, float k)
{
  return Quaternion{ lhs.x_ * k, lhs.y_ * k, lhs.z_ * k, lhs.w_ * k };
}

/* Usage : q3 = k * rhs */
inline Quaternion operator*(float k, const Quaternion& rhs)
{
  return rhs * k;
}

/* Usage : q3 = Add(lhs, rhs) */
inline Quaternion Add(const Quaternion& lhs, const Quaternion& rhs)
{
  return lhs + rhs;
}

/* Usage : q3 = Subtract(lhs, rhs) */
inline Quaternion Subtract(const Quaternion& lhs, const Quaternion& rhs)
{
  return lhs - rhs;
}

/* Usage : q3 = Multiply(lhs, rhs) */
inline Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
  return lhs * rhs;
}

/* Usage : q3 = Multiply(lhs, rhs) */
inline Vec3 Multiply(const Quaternion& lhs, const Vec3& v)
{
  auto result = v;
  lhs.Rotate(result);
  return result;
}

/* Usage : q3 = MultiplyT(lhs, rhs) */
inline Quaternion MultiplyT(const Quaternion& lhs, const Quaternion& rhs)
{
  return lhs.Conjugate() * rhs;
}

/* Usage : v = MultiplyT(lhs, rhs) */
inline Vec3 MultiplyT(const Quaternion& lhs, const Vec3& rhs)
{
  auto v = rhs;
  lhs.Conjugate().Rotate(v);
  return v;
}

inline Quaternion Scale(const Quaternion& lhs, float k)
{
  return lhs * k;
}

inline Quaternion Derivative(const Quaternion& orientation, const Vec3& velocity)
{
  Quaternion result;
  result.x_ = 0.5f * velocity.x_;
  result.y_ = 0.5f * velocity.y_;
  result.z_ = 0.5f * velocity.z_;
  result.w_ = 0.0f;

  return result * orientation;
}

/* Convert an axis and angle in degree into a quaternion object */
inline Quaternion ConvertAxisAngleToQuaternion(Vec3 axis, float angle)
{
  // q = [(sin(0.5 * angle)) * v_normalize, cos(0.5 * angle)] - representation of a quaternion with axis and angle 
  auto radian = Math::ConvertAngleToRadian(angle);
  radian *= 0.5f;
  axis.Normalize();

  auto sinAngle = sin(radian);

  return Quaternion{ axis * sinAngle, cos(radian) };
}

inline void ConvertAxisAngleToQuaternion(void* a, void* out, float angle)
{
  // q = [(sin(0.5 * angle)) * v_normalize, cos(0.5 * angle)] - representation of a quaternion with axis and angle 
  Quaternion * o = (Quaternion*)out;
  Vec3 * axis = (Vec3*)a;
  auto radian = Math::ConvertAngleToRadian(angle);
  radian *= 0.5f;
  axis->Normalize();

  auto sinAngle = sin(radian);
  *o = Quaternion{ *axis * sinAngle, cos(radian) };
}

inline Quaternion ConvertMat33ToQuaternion(const Matrix3& mat)
{
  auto trace = mat.m_[0][0] + mat.m_[1][1] + mat.m_[2][2];

  if (trace > 0.0f)
  {
    auto w = sqrtf(trace + 1) * 2;
    auto invw = 1 / w;
    return Quaternion((mat.m_[1][2] - mat.m_[2][1]) * invw,
      (mat.m_[2][0] - mat.m_[0][2]) * invw,
      (mat.m_[0][1] - mat.m_[1][0]) * invw,
      0.25f * w);
  }
  else if (mat.m_[0][0] > mat.m_[1][1] && mat.m_[0][0] > mat.m_[2][2])
  {
    auto w = sqrtf(mat.m_[0][0] - mat.m_[1][1] - mat.m_[2][2] + 1) * 2;
    auto invw = 1 / w;
    return Quaternion(0.25f * w,
      (mat.m_[0][1] + mat.m_[1][0]) * invw,
      (mat.m_[2][0] + mat.m_[0][2]) * invw,
      (mat.m_[1][2] - mat.m_[2][1]) * invw);
  }
  else if (mat.m_[1][1] > mat.m_[2][2])
  {
    auto w = sqrtf(mat.m_[1][1] - mat.m_[0][0] - mat.m_[2][2] + 1) * 2;
    auto invw = 1 / w;
    return Quaternion((mat.m_[0][1] + mat.m_[1][0]) * invw,
      0.25f * w,
      (mat.m_[1][2] + mat.m_[2][1]) * invw,
      (mat.m_[2][0] - mat.m_[0][2]) * invw);
  }
  else
  {
    auto w = sqrtf(mat.m_[2][2] - mat.m_[0][0] - mat.m_[1][1] + 1) * 2;
    auto invw = 1 / w;
    return Quaternion((mat.m_[2][0] + mat.m_[0][2]) * invw,
      (mat.m_[1][2] + mat.m_[2][1]) * invw,
      0.25f * w,
      (mat.m_[0][1] - mat.m_[1][0]) * invw);
  }
}

inline Quaternion ConvertEulerToQuaternion(const Vec3& euler)
{
  auto mat = ConvertEulerToMat33(euler);
  return ConvertMat33ToQuaternion(mat);
}

/* Convert a quaternion object into its axis vector and angle form */
inline void ConvertQuaternionToAxisAngle(Quaternion quat, Vec3& outVec, float& angle)
{
  quat.Normalize();

  auto sinAngle = sqrtf(1.0f - quat.w_ * quat.w_);

  if (fabsf(sinAngle) < FLT_EPSILON)
    sinAngle = 1.0f;

  outVec = Vec3{ quat.x_ / sinAngle, quat.y_ / sinAngle, quat.z_ / sinAngle };

  angle = Math::ConvertRadianToAngle(acos(quat.w_) * 2.0f);
}

/* Convert a quaternion object into its axis vector and angle form */
inline void ConvertQuaternionToAxisAngle(Quaternion* quat, Vec3* outVec, float* angle)
{
  quat->Normalize();

  auto sinAngle = sqrtf(1.0f - quat->w_ * quat->w_);

  if (fabsf(sinAngle) < FLT_EPSILON)
    sinAngle = 1.0f;

  *outVec = Vec3{ quat->x_ / sinAngle, quat->y_ / sinAngle, quat->z_ / sinAngle };

  *angle = Math::ConvertRadianToAngle(acos(quat->w_) * 2.0f);
}

inline Matrix4 ConvertQuaternionToMat44(Quaternion quat)
{
  Matrix4 result;
  float* matrix = result.m_[0];

  auto xx = quat.x_ * quat.x_;
  auto yy = quat.y_ * quat.y_;
  auto zz = quat.z_ * quat.z_;
  auto xy = quat.x_ * quat.y_;
  auto xz = quat.x_ * quat.z_;
  auto yz = quat.y_ * quat.z_;
  auto wx = quat.w_ * quat.x_;
  auto wy = quat.w_ * quat.y_;
  auto wz = quat.w_ * quat.z_;

  matrix[0] = 1.0f - 2.0f * (yy + zz);
  matrix[1] = 2.0f * (xy + wz);
  matrix[2] = 2.0f * (xz - wy);
  matrix[3] = 0.0f;

  matrix[4] = 2.0f * (xy - wz);
  matrix[5] = 1.0f - 2.0f * (xx + zz);
  matrix[6] = 2.0f * (yz + wx);
  matrix[7] = 0.0f;

  matrix[8] = 2.0f * (xz + wy);
  matrix[9] = 2.0f * (yz - wx);
  matrix[10] = 1.0f - 2.0f * (xx + yy);
  matrix[11] = 0.0f;

  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;

  return result;
}

inline Matrix3 ConvertQuaternionToMat33(const Quaternion& quat)
{
  Matrix3 result;
  float* matrix = result.m_[0];

  auto xx = quat.x_ * quat.x_;
  auto yy = quat.y_ * quat.y_;
  auto zz = quat.z_ * quat.z_;
  auto xy = quat.x_ * quat.y_;
  auto xz = quat.x_ * quat.z_;
  auto yz = quat.y_ * quat.z_;
  auto wx = quat.w_ * quat.x_;
  auto wy = quat.w_ * quat.y_;
  auto wz = quat.w_ * quat.z_;

  matrix[0] = 1.0f - 2.0f * (yy + zz);
  matrix[1] = 2.0f * (xy + wz);
  matrix[2] = 2.0f * (xz - wy);

  matrix[3] = 2.0f * (xy - wz);
  matrix[4] = 1.0f - 2.0f * (xx + zz);
  matrix[5] = 2.0f * (yz + wx);

  matrix[6] = 2.0f * (xz + wy);
  matrix[7] = 2.0f * (yz - wx);
  matrix[8] = 1.0f - 2.0f * (xx + yy);

  return result;
}

inline float Dot(const Quaternion& lhs, const Quaternion& rhs)
{
  return lhs.x_ * rhs.x_ + lhs.y_ * rhs.y_ + lhs.z_ * rhs.z_ + lhs.w_ * rhs.w_;
}

inline Vec3 ToEuler(const Quaternion& quat)
{
  Vec3 euler;

  float sinr_cosp = 2.f * (quat.w_ * quat.x_ + quat.y_ * quat.z_);
  float cosr_sinp = 1.f - 2.f * (quat.x_ * quat.x_ + quat.y_ * quat.y_);
  euler.x_ = Math::ConvertRadianToAngle(atan2f(sinr_cosp, cosr_sinp));

  float sinp = 2.f * (quat.w_ * quat.y_ - quat.z_ * quat.x_);
  if (fabs(sinp) >= 1.f)
  {
    euler.y_ = Math::ConvertRadianToAngle(copysignf(PI / 2.f, sinp));
  }
  else
  {
    euler.y_ = Math::ConvertRadianToAngle(asinf(sinp));
  }

  float siny_cosp = 2.f * (quat.w_ * quat.z_ + quat.x_ * quat.y_);
  float cosy_sinp = 1.f - 2.f * (quat.y_ * quat.y_ + quat.z_ * quat.z_);

  euler.z_ = Math::ConvertRadianToAngle(atan2f(siny_cosp, cosy_sinp));

  return euler;
}
// ------------------------------------------------------------- //
//
// Interpolates two quaternions
// Makes use of NLerp (Normalized Linear Interpolation) rather than 
// linearly interpolating between p0 and p1
//
// TODO: Convert to SLERP (Spherical Linear-Interpolation)
// ------------------------------------------------------------- //
template <>
inline Quaternion Math::Interpolate(const Quaternion& p0, const Quaternion& p1, float progression)
{
  Quaternion result;
  float dot = Dot(p0, p1);
  float inverseProgression = 1.f - progression;
  // Does not take the shorter path if normal interpolation used
  if (dot < 0)
  {
    result.w_ = inverseProgression * p0.w_ - progression * p1.w_;
    result.x_ = inverseProgression * p0.x_ - progression * p1.x_;
    result.y_ = inverseProgression * p0.y_ - progression * p1.y_;
    result.z_ = inverseProgression * p0.z_ - progression * p1.z_;
  }
  else
  {
    result.w_ = inverseProgression * p0.w_ + progression * p1.w_;
    result.x_ = inverseProgression * p0.x_ + progression * p1.x_;
    result.y_ = inverseProgression * p0.y_ + progression * p1.y_;
    result.z_ = inverseProgression * p0.z_ + progression * p1.z_;
  }
  result.Normalize();
  return result;
}

inline Quaternion LookRotation(const Vec3& dir, const Vec3& up = VECTOR_Y)
{
  auto up_vec = up;

  // dir and up cannot be parallel
  if (SqLength(Cross(dir, up_vec)) < FLT_EPSILON)
    up_vec = VECTOR_Z;

  auto forward = Normalize(dir);

  auto right = Cross(up_vec, forward);
  right.Normalize();

  up_vec = Cross(forward, right);

  auto mat = Matrix3{};
  auto m = &mat.m_[0][0];

  m[0] = right.x_;
  m[1] = right.y_;
  m[2] = right.z_;
  m[3] = up_vec.x_;
  m[4] = up_vec.y_;
  m[5] = up_vec.z_;
  m[6] = forward.x_;
  m[7] = forward.y_;
  m[8] = forward.z_;

  auto result = ConvertMat33ToQuaternion(mat);

  return ConvertMat33ToQuaternion(mat);
}
