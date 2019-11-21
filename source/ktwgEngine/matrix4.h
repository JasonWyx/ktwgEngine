/******************************************************************************/
/*!
\file       Matrix4.h
\author     Low Jin Liang Aaron
\par        Project: BPEngine
\brief      This is the interface file for Matrix4.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

// ------------------- Includes --------------- //
#include "Matrix3.h"

struct Matrix4;
struct Quaternion;

Matrix4 operator* (const Matrix4& lhs, const Matrix4 &rhs);

// -------------- Matrix 4 Row major implementation (Rows stored in contiguous memory) -------------- //
/*

 0  1  2  3
 4  5  6  7
 8  9 10 11
12 13 14 15

*/
struct Matrix4
{
  float m_[4][4];

  constexpr Matrix4() noexcept
    :m_{}
  {
  }

  constexpr Matrix4(float m00, float m01, float m02, float m03,
    float m04, float m05, float m06, float m07,
    float m08, float m09, float m10, float m11,
    float m12, float m13, float m14, float m15)
    : m_{ m00, m01, m02, m03, m04, m05, m06, m07, m08, m09, m10, m11, m12, m13, m14, m15 }
  {
  }

  void SetZero()
  {
    for (auto i = 0; i < 4; ++i)
    {
      for (auto j = 0; j < 4; ++j)
      {
        m_[i][j] = 0.f;
      }
    }
  }

  void SetIdentity()
  {
    for (auto i = 0; i < 4; ++i)
    {
      for (auto j = 0; j < 4; ++j)
      {
        if (i == j)
          m_[i][j] = 1.f;
        else
          m_[i][j] = 0.f;
      }
    }
  }

  void Swap(const Matrix4& rhs)
  {
    for (auto i = 0; i < 4; ++i)
    {
      for (auto j = 0; j < 4; ++j)
      {
        m_[i][j] = rhs.m_[i][j];
      }
    }
  }

  Matrix4 Transpose() const
  {
    return Matrix4{ m_[0][0], m_[1][0], m_[2][0], m_[3][0],
                      m_[0][1], m_[1][1], m_[2][1], m_[3][1],
                      m_[0][2], m_[1][2], m_[2][2], m_[3][2],
                      m_[0][3], m_[1][3], m_[2][3], m_[3][3] };
  }

  Matrix4& operator*= (const Matrix4 &rhs)
  {
    *this = *this * rhs;
    return *this;
  }

  Matrix4& operator*= (float rhs)
  {
    m_[0][0] *= rhs;
    m_[0][1] *= rhs;
    m_[0][2] *= rhs;
    m_[0][3] *= rhs;
    m_[1][0] *= rhs;
    m_[1][1] *= rhs;
    m_[1][2] *= rhs;
    m_[1][3] *= rhs;
    m_[2][0] *= rhs;
    m_[2][1] *= rhs;
    m_[2][2] *= rhs;
    m_[2][3] *= rhs;
    m_[3][0] *= rhs;
    m_[3][1] *= rhs;
    m_[3][2] *= rhs;
    m_[3][3] *= rhs;

    return *this;
  }

  Matrix4& operator/= (float rhs)
  {
    m_[0][0] /= rhs;
    m_[0][1] /= rhs;
    m_[0][2] /= rhs;
    m_[0][3] /= rhs;
    m_[1][0] /= rhs;
    m_[1][1] /= rhs;
    m_[1][2] /= rhs;
    m_[1][3] /= rhs;
    m_[2][0] /= rhs;
    m_[2][1] /= rhs;
    m_[2][2] /= rhs;
    m_[2][3] /= rhs;
    m_[3][0] /= rhs;
    m_[3][1] /= rhs;
    m_[3][2] /= rhs;
    m_[3][3] /= rhs;

    return *this;
  }

  Matrix4& operator+=(const Matrix4& rhs)
  {
    m_[0][0] += rhs.m_[0][0];
    m_[0][1] += rhs.m_[0][1];
    m_[0][2] += rhs.m_[0][2];
    m_[0][3] += rhs.m_[0][3];
    m_[1][0] += rhs.m_[1][0];
    m_[1][1] += rhs.m_[1][1];
    m_[1][2] += rhs.m_[1][2];
    m_[1][3] += rhs.m_[1][3];
    m_[2][0] += rhs.m_[2][0];
    m_[2][1] += rhs.m_[2][1];
    m_[2][2] += rhs.m_[2][2];
    m_[2][3] += rhs.m_[2][3];
    m_[3][0] += rhs.m_[3][0];
    m_[3][1] += rhs.m_[3][1];
    m_[3][2] += rhs.m_[3][2];
    m_[3][3] += rhs.m_[3][3];

    return *this;
  }

  Matrix4& operator-=(const Matrix4& rhs)
  {
    m_[0][0] -= rhs.m_[0][0];
    m_[0][1] -= rhs.m_[0][1];
    m_[0][2] -= rhs.m_[0][2];
    m_[0][3] -= rhs.m_[0][3];
    m_[1][0] -= rhs.m_[1][0];
    m_[1][1] -= rhs.m_[1][1];
    m_[1][2] -= rhs.m_[1][2];
    m_[1][3] -= rhs.m_[1][3];
    m_[2][0] -= rhs.m_[2][0];
    m_[2][1] -= rhs.m_[2][1];
    m_[2][2] -= rhs.m_[2][2];
    m_[2][3] -= rhs.m_[2][3];
    m_[3][0] -= rhs.m_[3][0];
    m_[3][1] -= rhs.m_[3][1];
    m_[3][2] -= rhs.m_[3][2];
    m_[3][3] -= rhs.m_[3][3];

    return *this;
  }
};

inline Matrix4 operator* (const Matrix4& lhs, const Matrix4 &rhs)
{
  Matrix4 ret;

  ret.m_[0][0] = lhs.m_[0][0] * rhs.m_[0][0] + lhs.m_[0][1] * rhs.m_[1][0] + lhs.m_[0][2] * rhs.m_[2][0] + lhs.m_[0][3] * rhs.m_[3][0];
  ret.m_[0][1] = lhs.m_[0][0] * rhs.m_[0][1] + lhs.m_[0][1] * rhs.m_[1][1] + lhs.m_[0][2] * rhs.m_[2][1] + lhs.m_[0][3] * rhs.m_[3][1];
  ret.m_[0][2] = lhs.m_[0][0] * rhs.m_[0][2] + lhs.m_[0][1] * rhs.m_[1][2] + lhs.m_[0][2] * rhs.m_[2][2] + lhs.m_[0][3] * rhs.m_[3][2];
  ret.m_[0][3] = lhs.m_[0][0] * rhs.m_[0][3] + lhs.m_[0][1] * rhs.m_[1][3] + lhs.m_[0][2] * rhs.m_[2][3] + lhs.m_[0][3] * rhs.m_[3][3];
  ret.m_[1][0] = lhs.m_[1][0] * rhs.m_[0][0] + lhs.m_[1][1] * rhs.m_[1][0] + lhs.m_[1][2] * rhs.m_[2][0] + lhs.m_[1][3] * rhs.m_[3][0];
  ret.m_[1][1] = lhs.m_[1][0] * rhs.m_[0][1] + lhs.m_[1][1] * rhs.m_[1][1] + lhs.m_[1][2] * rhs.m_[2][1] + lhs.m_[1][3] * rhs.m_[3][1];
  ret.m_[1][2] = lhs.m_[1][0] * rhs.m_[0][2] + lhs.m_[1][1] * rhs.m_[1][2] + lhs.m_[1][2] * rhs.m_[2][2] + lhs.m_[1][3] * rhs.m_[3][2];
  ret.m_[1][3] = lhs.m_[1][0] * rhs.m_[0][3] + lhs.m_[1][1] * rhs.m_[1][3] + lhs.m_[1][2] * rhs.m_[2][3] + lhs.m_[1][3] * rhs.m_[3][3];
  ret.m_[2][0] = lhs.m_[2][0] * rhs.m_[0][0] + lhs.m_[2][1] * rhs.m_[1][0] + lhs.m_[2][2] * rhs.m_[2][0] + lhs.m_[2][3] * rhs.m_[3][0];
  ret.m_[2][1] = lhs.m_[2][0] * rhs.m_[0][1] + lhs.m_[2][1] * rhs.m_[1][1] + lhs.m_[2][2] * rhs.m_[2][1] + lhs.m_[2][3] * rhs.m_[3][1];
  ret.m_[2][2] = lhs.m_[2][0] * rhs.m_[0][2] + lhs.m_[2][1] * rhs.m_[1][2] + lhs.m_[2][2] * rhs.m_[2][2] + lhs.m_[2][3] * rhs.m_[3][2];
  ret.m_[2][3] = lhs.m_[2][0] * rhs.m_[0][3] + lhs.m_[2][1] * rhs.m_[1][3] + lhs.m_[2][2] * rhs.m_[2][3] + lhs.m_[2][3] * rhs.m_[3][3];
  ret.m_[3][0] = lhs.m_[3][0] * rhs.m_[0][0] + lhs.m_[3][1] * rhs.m_[1][0] + lhs.m_[3][2] * rhs.m_[2][0] + lhs.m_[3][3] * rhs.m_[3][0];
  ret.m_[3][1] = lhs.m_[3][0] * rhs.m_[0][1] + lhs.m_[3][1] * rhs.m_[1][1] + lhs.m_[3][2] * rhs.m_[2][1] + lhs.m_[3][3] * rhs.m_[3][1];
  ret.m_[3][2] = lhs.m_[3][0] * rhs.m_[0][2] + lhs.m_[3][1] * rhs.m_[1][2] + lhs.m_[3][2] * rhs.m_[2][2] + lhs.m_[3][3] * rhs.m_[3][2];
  ret.m_[3][3] = lhs.m_[3][0] * rhs.m_[0][3] + lhs.m_[3][1] * rhs.m_[1][3] + lhs.m_[3][2] * rhs.m_[2][3] + lhs.m_[3][3] * rhs.m_[3][3];

  return ret;
}

inline Matrix4 operator*(const Matrix4& lhs, float rhs)
{
  Matrix4 res;

  res.m_[0][0] = lhs.m_[0][0] * rhs;
  res.m_[0][1] = lhs.m_[0][1] * rhs;
  res.m_[0][2] = lhs.m_[0][2] * rhs;
  res.m_[0][3] = lhs.m_[0][3] * rhs;
  res.m_[1][0] = lhs.m_[1][0] * rhs;
  res.m_[1][1] = lhs.m_[1][1] * rhs;
  res.m_[1][2] = lhs.m_[1][2] * rhs;
  res.m_[1][3] = lhs.m_[1][3] * rhs;
  res.m_[2][0] = lhs.m_[2][0] * rhs;
  res.m_[2][1] = lhs.m_[2][1] * rhs;
  res.m_[2][2] = lhs.m_[2][2] * rhs;
  res.m_[2][3] = lhs.m_[2][3] * rhs;
  res.m_[3][0] = lhs.m_[3][0] * rhs;
  res.m_[3][1] = lhs.m_[3][1] * rhs;
  res.m_[3][2] = lhs.m_[3][2] * rhs;
  res.m_[3][3] = lhs.m_[3][3] * rhs;

  return res;
}

inline Matrix4 operator*(float lhs, const Matrix4& rhs)
{
  return rhs * lhs;
}

inline Matrix4 operator/(const Matrix4& lhs, float rhs)
{
  Matrix4 res;

  res.m_[0][0] = lhs.m_[0][0] / rhs;
  res.m_[0][1] = lhs.m_[0][1] / rhs;
  res.m_[0][2] = lhs.m_[0][2] / rhs;
  res.m_[0][3] = lhs.m_[0][3] / rhs;
  res.m_[1][0] = lhs.m_[1][0] / rhs;
  res.m_[1][1] = lhs.m_[1][1] / rhs;
  res.m_[1][2] = lhs.m_[1][2] / rhs;
  res.m_[1][3] = lhs.m_[1][3] / rhs;
  res.m_[2][0] = lhs.m_[2][0] / rhs;
  res.m_[2][1] = lhs.m_[2][1] / rhs;
  res.m_[2][2] = lhs.m_[2][2] / rhs;
  res.m_[2][3] = lhs.m_[2][3] / rhs;
  res.m_[3][0] = lhs.m_[3][0] / rhs;
  res.m_[3][1] = lhs.m_[3][1] / rhs;
  res.m_[3][2] = lhs.m_[3][2] / rhs;
  res.m_[3][3] = lhs.m_[3][3] / rhs;

  return res;
}

inline Matrix4 operator+(const Matrix4& lhs, const Matrix4& rhs)
{
  Matrix4 ret;

  ret.m_[0][0] = lhs.m_[0][0] + rhs.m_[0][0];
  ret.m_[0][1] = lhs.m_[0][1] + rhs.m_[0][1];
  ret.m_[0][2] = lhs.m_[0][2] + rhs.m_[0][2];
  ret.m_[0][3] = lhs.m_[0][3] + rhs.m_[0][3];
  ret.m_[1][0] = lhs.m_[1][0] + rhs.m_[1][0];
  ret.m_[1][1] = lhs.m_[1][1] + rhs.m_[1][1];
  ret.m_[1][2] = lhs.m_[1][2] + rhs.m_[1][2];
  ret.m_[1][3] = lhs.m_[1][3] + rhs.m_[1][3];
  ret.m_[2][0] = lhs.m_[2][0] + rhs.m_[2][0];
  ret.m_[2][1] = lhs.m_[2][1] + rhs.m_[2][1];
  ret.m_[2][2] = lhs.m_[2][2] + rhs.m_[2][2];
  ret.m_[2][3] = lhs.m_[2][3] + rhs.m_[2][3];
  ret.m_[3][0] = lhs.m_[3][0] + rhs.m_[3][0];
  ret.m_[3][1] = lhs.m_[3][1] + rhs.m_[3][1];
  ret.m_[3][2] = lhs.m_[3][2] + rhs.m_[3][2];
  ret.m_[3][3] = lhs.m_[3][3] + rhs.m_[3][3];

  return ret;
}

inline Matrix4 operator-(const Matrix4& lhs, const Matrix4& rhs)
{
  Matrix4 ret;

  ret.m_[0][0] = lhs.m_[0][0] - rhs.m_[0][0];
  ret.m_[0][1] = lhs.m_[0][1] - rhs.m_[0][1];
  ret.m_[0][2] = lhs.m_[0][2] - rhs.m_[0][2];
  ret.m_[0][3] = lhs.m_[0][3] - rhs.m_[0][3];
  ret.m_[1][0] = lhs.m_[1][0] - rhs.m_[1][0];
  ret.m_[1][1] = lhs.m_[1][1] - rhs.m_[1][1];
  ret.m_[1][2] = lhs.m_[1][2] - rhs.m_[1][2];
  ret.m_[1][3] = lhs.m_[1][3] - rhs.m_[1][3];
  ret.m_[2][0] = lhs.m_[2][0] - rhs.m_[2][0];
  ret.m_[2][1] = lhs.m_[2][1] - rhs.m_[2][1];
  ret.m_[2][2] = lhs.m_[2][2] - rhs.m_[2][2];
  ret.m_[2][3] = lhs.m_[2][3] - rhs.m_[2][3];
  ret.m_[3][0] = lhs.m_[3][0] - rhs.m_[3][0];
  ret.m_[3][1] = lhs.m_[3][1] - rhs.m_[3][1];
  ret.m_[3][2] = lhs.m_[3][2] - rhs.m_[3][2];
  ret.m_[3][3] = lhs.m_[3][3] - rhs.m_[3][3];

  return ret;
}

inline bool operator==(const Matrix4& lhs, const Matrix4& rhs)
{
  return
    lhs.m_[0][0] == rhs.m_[0][0] &&
    lhs.m_[0][1] == rhs.m_[0][1] &&
    lhs.m_[0][2] == rhs.m_[0][2] &&
    lhs.m_[0][3] == rhs.m_[0][3] &&
    lhs.m_[1][0] == rhs.m_[1][0] &&
    lhs.m_[1][1] == rhs.m_[1][1] &&
    lhs.m_[1][2] == rhs.m_[1][2] &&
    lhs.m_[1][3] == rhs.m_[1][3] &&
    lhs.m_[2][0] == rhs.m_[2][0] &&
    lhs.m_[2][1] == rhs.m_[2][1] &&
    lhs.m_[2][2] == rhs.m_[2][2] &&
    lhs.m_[2][3] == rhs.m_[2][3] &&
    lhs.m_[3][0] == rhs.m_[3][0] &&
    lhs.m_[3][1] == rhs.m_[3][1] &&
    lhs.m_[3][2] == rhs.m_[3][2] &&
    lhs.m_[3][3] == rhs.m_[3][3];
}

inline bool operator!=(const Matrix4& lhs, const Matrix4& rhs)
{
  return !(lhs == rhs);
}

inline Matrix4 OuterProductMat4(const Vec3& lhs, const Vec3& rhs)
{
  auto a = rhs * lhs.x_;
  auto b = rhs * lhs.y_;
  auto c = rhs * lhs.z_;

  return Matrix4(a.x_, a.y_, a.z_, 0.0f,
    b.x_, b.y_, b.z_, 0.0f,
    c.x_, c.y_, c.z_, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix3 ConvertMat4ToMat3(const Matrix4& m44)
{
  return Matrix3{ m44.m_[0][0], m44.m_[0][1], m44.m_[0][2],
                 m44.m_[1][0], m44.m_[1][1], m44.m_[1][2],
                 m44.m_[2][0], m44.m_[2][1], m44.m_[2][2] };
}

inline Matrix4 ConvertMat3ToMat4(const Matrix3& m33)
{
  return Matrix4{ m33.m_[0][0], m33.m_[0][1], m33.m_[0][2], 0.0f,
                 m33.m_[1][0], m33.m_[1][1], m33.m_[1][2], 0.0f,
                 m33.m_[2][0], m33.m_[2][1], m33.m_[2][2], 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f };
}

inline Matrix4& GetPerspectiveMatrixLH(Matrix4& matrix, float fov, float aspect, float zn, float zf)
{
  matrix.SetZero();
  float yScale = 1.f / tanf(fov / 2.0f);
  float xScale = yScale / aspect;

  matrix.m_[0][0] = xScale;
  matrix.m_[1][1] = yScale;
  matrix.m_[2][2] = zf / (zf - zn);
  matrix.m_[2][3] = 1;
  matrix.m_[3][2] = -(zn * zf / (zf - zn));

  return matrix;
}

inline Matrix4& GetOrthogonalMatrixLH(Matrix4& matrix, float width, float height, float zn, float zf)
{
  matrix.SetZero();
  matrix.m_[0][0] = 2.f / width;
  matrix.m_[1][1] = 2.f / height;
  matrix.m_[2][2] = 1.f / (zf - zn);
  matrix.m_[3][2] = -zn / (zf - zn);
  matrix.m_[3][3] = 1.f;

  return matrix;
}

inline Matrix4& LookAtMatrix(Matrix4& matrix, const Vec3& eye, const Vec3& at, const Vec3& up)
{
  auto z = at - eye;
  z.Normalize();
  auto x = Cross(up, z);
  x.Normalize();
  auto y = Cross(z, x);
  auto pos = eye;
  matrix = Matrix4{ x.x_, y.x_, z.x_, 0.f,
            x.y_, y.y_, z.y_, 0.f,
            x.z_, y.z_, z.z_, 0.f,
           -Dot(x, pos), -Dot(y, pos), -Dot(z, pos), 1.f };
  return matrix;
}

// Inverse of a unit matrix
inline Matrix4& UnitInverse(Matrix4& matrix)
{
  Vec3 u = Vec3{ matrix.m_[0][0], matrix.m_[0][1], matrix.m_[0][2] };
  Vec3 v = Vec3{ matrix.m_[1][0], matrix.m_[1][1], matrix.m_[1][2] };
  Vec3 w = Vec3{ matrix.m_[2][0], matrix.m_[2][1], matrix.m_[2][2] };
  Vec3 p = Vec3{ matrix.m_[3][0], matrix.m_[3][1], matrix.m_[3][2] };

  matrix.m_[0][1] = matrix.m_[1][0];
  matrix.m_[0][2] = matrix.m_[2][0];
  matrix.m_[2][1] = matrix.m_[1][2];

  matrix.m_[3][0] = -Dot(u, p);
  matrix.m_[3][1] = -Dot(v, p);
  matrix.m_[3][2] = -Dot(w, p);

  return matrix;
}

inline Vec3 MulPoint(const Matrix4& matrix, const Vec3& point)
{
  Vec3 ret;

  ret.x_ = point.x_ * matrix.m_[0][0] + point.y_ * matrix.m_[1][0] + point.z_ * matrix.m_[2][0] + matrix.m_[3][0];
  ret.y_ = point.x_ * matrix.m_[0][1] + point.y_ * matrix.m_[1][1] + point.z_ * matrix.m_[2][1] + matrix.m_[3][1];
  ret.z_ = point.x_ * matrix.m_[0][2] + point.y_ * matrix.m_[1][2] + point.z_ * matrix.m_[2][2] + matrix.m_[3][2];

  return ret;
}

inline Vec3 MulPointDivideW(const Matrix4& matrix, const Vec3& point)
{
  Vec3 ret;

  auto invW = 1.f / (point.x_ * matrix.m_[0][3] + point.y_ * matrix.m_[1][3] + point.z_ * matrix.m_[2][3] + matrix.m_[3][3]);
  ret.x_ = invW * (point.x_ * matrix.m_[0][0] + point.y_ * matrix.m_[1][0] + point.z_ * matrix.m_[2][0] + matrix.m_[3][0]);
  ret.y_ = invW * (point.x_ * matrix.m_[0][1] + point.y_ * matrix.m_[1][1] + point.z_ * matrix.m_[2][1] + matrix.m_[3][1]);
  ret.z_ = invW * (point.x_ * matrix.m_[0][2] + point.y_ * matrix.m_[1][2] + point.z_ * matrix.m_[2][2] + matrix.m_[3][2]);
  return ret;
}

inline Vec3 MulVec(const Matrix4& matrix, const Vec3& vec)
{
  Vec3 ret;

  ret.x_ = vec.x_ * matrix.m_[0][0] + vec.y_ * matrix.m_[1][0] + vec.z_ * matrix.m_[2][0];
  ret.y_ = vec.x_ * matrix.m_[0][1] + vec.y_ * matrix.m_[1][1] + vec.z_ * matrix.m_[2][1];
  ret.z_ = vec.x_ * matrix.m_[0][2] + vec.y_ * matrix.m_[1][2] + vec.z_ * matrix.m_[2][2];

  return ret;
}

void Inverse(Matrix4 const& rhs, Matrix4& result);

void DecomposeMatrix(const Matrix4& matrix, Vec3& scale, Quaternion& quat, Vec3& pos);
