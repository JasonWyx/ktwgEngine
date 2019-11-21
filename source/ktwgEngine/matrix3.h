/******************************************************************************/
/*!
\file       Matrix3.h
\author     Low Jin Liang Aaron
\par        Project: BPEngine
\brief      This is the interface file for Matrix3.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

// ------------------- Includes --------------- //
#include "Vector3.h"
#include <algorithm>

#ifndef PI
#define PI 3.14159265359f
#endif

struct Matrix3;

Matrix3 operator* (const Matrix3& lhs, const Matrix3 &rhs);

// -------------- Matrix 3 Row major implementation (Rows stored in contiguous memory) -------------- //
/*

 0  1  2
 3  4  5
 6  7  8

*/
struct Matrix3
{
  float m_[3][3];

  constexpr Matrix3() noexcept
    :m_{}
  {
  }

  constexpr Matrix3(float m00, float m01, float m02,
    float m03, float m04, float m05,
    float m06, float m07, float m08)
    : m_{ m00, m01, m02, m03, m04, m05, m06, m07, m08 }
  {
  }

  void SetZero()
  {
    for (auto i = 0; i < 3; ++i)
    {
      for (auto j = 0; j < 3; ++j)
      {
        m_[i][j] = 0.f;
      }
    }
  }

  void SetIdentity()
  {
    for (auto i = 0; i < 3; ++i)
    {
      for (auto j = 0; j < 3; ++j)
      {
        m_[i][j] = i == j ? 1.f : 0.f;
      }
    }
  }

  void Swap(Matrix3& rhs)
  {
    for (auto i = 0; i < 3; ++i)
    {
      for (auto j = 0; j < 3; ++j)
      {
        std::swap(m_[i][j], rhs.m_[i][j]);
      }
    }
  }

  Matrix3 Transpose() const
  {
    return Matrix3{ m_[0][0], m_[1][0], m_[2][0],
                    m_[0][1], m_[1][1], m_[2][1],
                    m_[0][2], m_[1][2], m_[2][2],
    };
  }

  Matrix3& operator*= (const Matrix3 &rhs)
  {
    *this = *this * rhs;
    return *this;
  }

  Matrix3& operator*= (float rhs)
  {
    m_[0][0] *= rhs;
    m_[0][1] *= rhs;
    m_[0][2] *= rhs;
    m_[1][0] *= rhs;
    m_[1][1] *= rhs;
    m_[1][2] *= rhs;
    m_[2][0] *= rhs;
    m_[2][1] *= rhs;
    m_[2][2] *= rhs;

    return *this;
  }

  Matrix3& operator/= (float rhs)
  {
    m_[0][0] /= rhs;
    m_[0][1] /= rhs;
    m_[0][2] /= rhs;
    m_[1][0] /= rhs;
    m_[1][1] /= rhs;
    m_[1][2] /= rhs;
    m_[2][0] /= rhs;
    m_[2][1] /= rhs;
    m_[2][2] /= rhs;

    return *this;
  }

  Matrix3& operator+=(const Matrix3& rhs)
  {
    m_[0][0] += rhs.m_[0][0];
    m_[0][1] += rhs.m_[0][1];
    m_[0][2] += rhs.m_[0][2];
    m_[1][0] += rhs.m_[1][0];
    m_[1][1] += rhs.m_[1][1];
    m_[1][2] += rhs.m_[1][2];
    m_[2][0] += rhs.m_[2][0];
    m_[2][1] += rhs.m_[2][1];
    m_[2][2] += rhs.m_[2][2];

    return *this;
  }

  Matrix3& operator-=(const Matrix3& rhs)
  {
    m_[0][0] -= rhs.m_[0][0];
    m_[0][1] -= rhs.m_[0][1];
    m_[0][2] -= rhs.m_[0][2];
    m_[1][0] -= rhs.m_[1][0];
    m_[1][1] -= rhs.m_[1][1];
    m_[1][2] -= rhs.m_[1][2];
    m_[2][0] -= rhs.m_[2][0];
    m_[2][1] -= rhs.m_[2][1];
    m_[2][2] -= rhs.m_[2][2];

    return *this;
  }

  inline Vec3 GetColumn(unsigned index) const
  {
    return Vec3{ m_[0][index], m_[1][index], m_[2][index] };
  }

  inline Vec3 GetRow(unsigned index) const
  {
    return Vec3{ m_[index][0], m_[index][1], m_[index][2] };
  }

  inline void SetRows(const Vec3& row1, const Vec3& row2, const Vec3& row3)
  {
    m_[0][0] = row1.x_;
    m_[0][1] = row1.y_;
    m_[0][2] = row1.z_;
    m_[1][0] = row2.x_;
    m_[1][1] = row2.y_;
    m_[1][2] = row2.z_;
    m_[2][0] = row3.x_;
    m_[2][1] = row3.y_;
    m_[2][2] = row3.z_;
  }

  inline void SetColumns(const Vec3& col1, const Vec3& col2, const Vec3& col3)
  {
    m_[0][0] = col1.x_;
    m_[1][0] = col1.y_;
    m_[2][0] = col1.z_;
    m_[0][1] = col2.x_;
    m_[1][1] = col2.y_;
    m_[2][1] = col2.z_;
    m_[0][2] = col3.x_;
    m_[1][2] = col3.y_;
    m_[2][2] = col3.z_;
  }
};

inline Matrix3 operator* (const Matrix3& lhs, const Matrix3 &rhs)
{
  Matrix3 tmp;

  tmp.m_[0][0] = lhs.m_[0][0] * rhs.m_[0][0] + lhs.m_[0][1] * rhs.m_[1][0] + lhs.m_[0][2] * rhs.m_[2][0];
  tmp.m_[0][1] = lhs.m_[0][0] * rhs.m_[0][1] + lhs.m_[0][1] * rhs.m_[1][1] + lhs.m_[0][2] * rhs.m_[2][1];
  tmp.m_[0][2] = lhs.m_[0][0] * rhs.m_[0][2] + lhs.m_[0][1] * rhs.m_[1][2] + lhs.m_[0][2] * rhs.m_[2][2];
  tmp.m_[1][0] = lhs.m_[1][0] * rhs.m_[0][0] + lhs.m_[1][1] * rhs.m_[1][0] + lhs.m_[1][2] * rhs.m_[2][0];
  tmp.m_[1][1] = lhs.m_[1][0] * rhs.m_[0][1] + lhs.m_[1][1] * rhs.m_[1][1] + lhs.m_[1][2] * rhs.m_[2][1];
  tmp.m_[1][2] = lhs.m_[1][0] * rhs.m_[0][2] + lhs.m_[1][1] * rhs.m_[1][2] + lhs.m_[1][2] * rhs.m_[2][2];
  tmp.m_[2][0] = lhs.m_[2][0] * rhs.m_[0][0] + lhs.m_[2][1] * rhs.m_[1][0] + lhs.m_[2][2] * rhs.m_[2][0];
  tmp.m_[2][1] = lhs.m_[2][0] * rhs.m_[0][1] + lhs.m_[2][1] * rhs.m_[1][1] + lhs.m_[2][2] * rhs.m_[2][1];
  tmp.m_[2][2] = lhs.m_[2][0] * rhs.m_[0][2] + lhs.m_[2][1] * rhs.m_[1][2] + lhs.m_[2][2] * rhs.m_[2][2];

  return tmp;
}

inline Matrix3 operator*(const Matrix3& lhs, float rhs)
{
  Matrix3 res{ lhs };
  res *= rhs;
  return res;
}

inline Matrix3 operator*(float lhs, const Matrix3& rhs)
{
  return rhs * lhs;
}

inline Matrix3 operator/(const Matrix3& lhs, float rhs)
{
  Matrix3 res{ lhs };
  res /= rhs;
  return res;
}

inline Matrix3 operator+(const Matrix3& lhs, const Matrix3& rhs)
{
  Matrix3 res{ lhs };
  res += rhs;
  return res;
}

inline Matrix3 operator-(const Matrix3& lhs, const Matrix3& rhs)
{
  Matrix3 res{ lhs };
  res -= rhs;
  return res;
}

inline bool operator==(const Matrix3& lhs, const Matrix3& rhs)
{
  return
    lhs.m_[0][0] == rhs.m_[0][0] &&
    lhs.m_[0][1] == rhs.m_[0][1] &&
    lhs.m_[0][2] == rhs.m_[0][2] &&
    lhs.m_[1][0] == rhs.m_[1][0] &&
    lhs.m_[1][1] == rhs.m_[1][1] &&
    lhs.m_[1][2] == rhs.m_[1][2] &&
    lhs.m_[2][0] == rhs.m_[2][0] &&
    lhs.m_[2][1] == rhs.m_[2][1] &&
    lhs.m_[2][2] == rhs.m_[2][2];
}

inline bool operator!=(const Matrix3& lhs, const Matrix3& rhs)
{
  return !(lhs == rhs);
}

inline Matrix3 Inverse(const Matrix3& mat)
{
  Vec3 v0, v1, v2;
  float detInv;
  Vec3 x = Vec3{ mat.m_[0][0], mat.m_[0][1], mat.m_[0][2] };
  Vec3 y = Vec3{ mat.m_[1][0], mat.m_[1][1], mat.m_[1][2] };
  Vec3 z = Vec3{ mat.m_[2][0], mat.m_[2][1], mat.m_[2][2] };

  v0 = Cross(y, z);
  v1 = Cross(z, x);
  v2 = Cross(x, y);

  detInv = 1.0f / Dot(z, v2);

  Matrix3 result{ v0.x_ * detInv, v1.x_ * detInv, v2.x_ * detInv,
                  v0.y_ * detInv, v1.y_ * detInv, v2.y_ * detInv,
                  v0.z_ * detInv, v1.z_ * detInv, v2.z_ * detInv };

  return result;
}

inline Vec3 Multiply(const Matrix3& mat, const Vec3& vec)
{
  Vec3 result{};
  result.x_ = Dot(Vec3{ mat.m_[0][0], mat.m_[1][0], mat.m_[2][0] }, vec);
  result.y_ = Dot(Vec3{ mat.m_[0][1], mat.m_[1][1], mat.m_[2][1] }, vec);
  result.z_ = Dot(Vec3{ mat.m_[0][2], mat.m_[1][2], mat.m_[2][2] }, vec);

  return result;
}

inline Vec3 MultiplyT(const Matrix3& mat, const Vec3& vec)
{
  Vec3 result{};
  result.x_ = Dot(Vec3{ mat.m_[0][0], mat.m_[0][1], mat.m_[0][2] }, vec);
  result.y_ = Dot(Vec3{ mat.m_[1][0], mat.m_[1][1], mat.m_[1][2] }, vec);
  result.z_ = Dot(Vec3{ mat.m_[2][0], mat.m_[2][1], mat.m_[2][2] }, vec);

  return result;
}

inline Matrix3 OuterProductMat3(const Vec3& u, const Vec3& v)
{
  auto a = u.x_ * v;
  auto b = u.y_ * v;
  auto c = u.z_ * v;

  return Matrix3(a.x_, a.y_, a.z_,
    b.x_, b.y_, b.z_,
    c.x_, c.y_, c.z_);
}

inline Matrix3 RotateToFrame(const Matrix3& frame, const Matrix3& rotation)
{
  return rotation.Transpose() * frame * rotation;
}

inline Matrix3 ConvertEulerToMat33(const Vec3& euler)
{
  Matrix3 result;
  result.SetIdentity();
  auto toRad = PI / 180.f;
  auto radx = toRad * euler.x_;
  auto rady = toRad * euler.y_;
  auto radz = toRad * euler.z_;

  // Compute cosine and sine once
  auto cosx = cosf(radx);
  auto sinx = sinf(radx);
  auto cosy = cosf(rady);
  auto siny = sinf(rady);
  auto cosz = cosf(radz);
  auto sinz = sinf(radz);
  auto sinxsiny = sinx * siny;
  auto cosxsiny = cosx * siny;

  result.m_[0][0] = cosy * cosz;
  result.m_[0][1] = cosy * sinz;
  result.m_[0][2] = -siny;
  result.m_[1][0] = sinxsiny * cosz - cosx * sinz;
  result.m_[1][1] = sinxsiny * sinz + cosx * cosz;
  result.m_[1][2] = sinx * cosy;
  result.m_[2][0] = cosxsiny * cosz + sinx * sinz;
  result.m_[2][1] = cosxsiny * sinz - sinx * cosz;
  result.m_[2][2] = cosx * cosy;

  return result;
}
