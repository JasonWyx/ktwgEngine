#pragma once

#include "vector3.h"
#include "transform.h"
#include "quaternion.h"

namespace SAT
{
  enum SATType
  {
    eSAT_Edge,
    eSAT_FaceA,
    eSAT_FaceB
  };

  struct FaceQuery
  {
    float    maxSeparation_ = 0.0f;   // SAT separation value
    Vec3     normal_ = Vec3{}; // SAT normal used
    unsigned index_ = static_cast<unsigned>(-1);
  };

  struct EdgeQuery
  {
    float maxSeparation_ = 0.0f;   // SAT separation value
    Vec3 normal_ = Vec3{}; // SAT normal used
    unsigned index_ = static_cast<unsigned>(-1);
  };

  bool CheckFaceSeparation(FaceQuery& outA, FaceQuery& outB,
    const Transform& transformA, const Transform& transformB,
    const Vec3& extentsA, const Vec3& extentsB,
    const Matrix3& rotMatA, const Matrix3& rotMatB,
    const Matrix3& transform, const Matrix3& absoluteTransform);

  bool CheckEdgeSeparation(EdgeQuery& out,
    const Transform& transformA, const Transform& transformB,
    const Vec3& extentsA, const Vec3& extentsB,
    const Matrix3& rotMatA, const Matrix3& rotMatB,
    const Matrix3& transform, const Matrix3& absoluteTransform);

  bool CheckSeparation(FaceQuery& out, float separation, const Vec3& normal, unsigned index);
  bool CheckSeparation(EdgeQuery& out, float separation, const Vec3& normal, unsigned index);
}