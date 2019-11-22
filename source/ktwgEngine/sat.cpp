#include "sat.h"

bool SAT::CheckFaceSeparation(FaceQuery& outA, FaceQuery& outB, const Transform& transformA, const Transform& transformB, const Vec3& extentsA, const Vec3& extentsB, const Matrix3& rotMatA, const Matrix3& rotMatB, const Matrix3& transform, const Matrix3& absoluteTransform)
{
  Vec3 t = MultiplyT(rotMatA, transformB.GetPosition() - transformA.GetPosition());
  float s = 0.0f;

  float tmo = Dot(absoluteTransform.GetColumn(0), extentsB);
  // Check x axis of Box A, normal to face is also the x axis
  s = abs(t.x_) - (extentsA.x_ + Dot(absoluteTransform.GetColumn(0), extentsB));
  if (CheckSeparation(outA, s, rotMatA.GetRow(0), 0))
    return false;

  tmo = Dot(absoluteTransform.GetColumn(1), extentsB);
  // Check y axis of Box A, normal to face is also the y axis
  s = abs(t.y_) - (extentsA.y_ + Dot(absoluteTransform.GetColumn(1), extentsB));
  if (CheckSeparation(outA, s, rotMatA.GetRow(1), 1))
    return false;

  tmo = Dot(absoluteTransform.GetColumn(2), extentsB);
  // Check z axis of Box A, normal to face is also the z axis
  s = abs(t.z_) - (extentsA.z_ + Dot(absoluteTransform.GetColumn(2), extentsB));
  if (CheckSeparation(outA, s, rotMatA.GetRow(2), 2))
    return false;

  // Check x axis of Box B, normal to face is also the x axis
  s = abs(Dot(t, transform.GetRow(0))) - (extentsB.x_ + Dot(absoluteTransform.GetRow(0), extentsA));
  if (CheckSeparation(outB, s, rotMatB.GetRow(0), 3))
    return false;

  // Check y axis of Box B, normal to face is also the y axis
  s = abs(Dot(t, transform.GetRow(1))) - (extentsB.y_ + Dot(absoluteTransform.GetRow(1), extentsA));
  if (CheckSeparation(outB, s, rotMatB.GetRow(1), 4))
    return false;

  // Check z axis of Box B, normal to face is also the z axis
  s = abs(Dot(t, transform.GetRow(2))) - (extentsB.z_ + Dot(absoluteTransform.GetRow(2), extentsA));
  if (CheckSeparation(outB, s, rotMatB.GetRow(2), 5))
    return false;

  // Pass all face axis test
  return true;
}

bool SAT::CheckEdgeSeparation(EdgeQuery& out, const Transform& transformA, const Transform& transformB, const Vec3& extentsA, const Vec3& extentsB, const Matrix3& rotMatA, const Matrix3& rotMatB, const Matrix3& transform, const Matrix3& absoluteTransform)
{
  // Compute vector t from center A to center B in A's space
  Vec3 t = MultiplyT(rotMatA, transformB.GetPosition() - transformA.GetPosition());
  float s = 0.0f;

  // In A's space, A's local axis is (1, 0, 0), (0, 1, 0) and (0, 0, 1)
  // In A's space, B's axis is (A^T * B)'s row 0, row 1 and row 2 ( row major )

  // Edge case : Cross A.x & B.x
  // l = Cross(A.x, B.x) = (1, 0, 0) x | (A^T * B).row(0) | = (0, -m[0][2], m[0][1])
  // | A^T * B | = absoluteTransform
  // s = Dot(t, l) - Dot(extentA, l) - Dot(absTransform * extentB, l)
  Vec3 l = Vec3{ 0.0f, -transform.m_[0][2], transform.m_[0][1] };
  Vec3 absL = Absolute(l);
  float projA = Dot(extentsA, absL);
  float projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 6))
    return false;

  // Edge case : Cross A.x & B.y
  // l = Cross(A.x, B.y) = (1, 0, 0) x | (A^T * B).row(1) | = (0, -m[1][2], m[1][1])
  l = Vec3{ 0.0f, -transform.m_[1][2], transform.m_[1][1] };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 7))
    return false;

  // Edge case : Cross A.x & B.z
  // l = Cross(A.x, B.z) = (1, 0, 0) x | (A^T * B).row(2) | = (0, -m[2][2], m[2][1])
  l = Vec3{ 0.0f, -transform.m_[2][2], transform.m_[2][1] };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 8))
    return false;

  // Edge case : Cross A.y & B.x
  // l = Cross(A.y, B.x) = (0, 1, 0) x | (A^T * B).row(0) | = (m[0][2], 0, -m[0][0])
  l = Vec3{ transform.m_[0][2], 0.0f, -transform.m_[0][0] };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 9))
    return false;

  // Edge case : Cross A.y & B.y
  // l = Cross(A.y, B.y) = (0, 1, 0) x | (A^T * B).row(1) | = (m[1][2], 0, -m[1][0])
  l = Vec3{ transform.m_[1][2], 0.0f, -transform.m_[1][0] };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 10))
    return false;

  // Edge case : Cross A.y & B.z
  // l = Cross(A.y, B.z) = (0, 1, 0) x | (A^T * B).row(2) | = (m[2][2], 0, -Column[2][0])
  l = Vec3{ transform.m_[2][2], 0.0f, -transform.m_[2][0] };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 11))
    return false;

  // Edge case : Cross A.z & B.x
  // l = Cross(A.z, B.x) = (0, 0, 1) x | (A^T * B).row(0) | = (-m[0][1], Column[0][0], 0.0f)
  l = Vec3{ -transform.m_[0][1], transform.m_[0][0], 0.0f };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 12))
    return false;

  // Edge case : Cross A.z & B.y
  // l = Cross(A.z, B.x) = (0, 0, 1) x | (A^T * B).row(1) | = (-m[1][1], m[1][0], 0.0f)
  l = Vec3{ -transform.m_[1][1], transform.m_[1][0], 0.0f };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 13))
    return false;

  // Edge case : Cross A.z & B.z
  // l = Cross(A.z, B.x) = (0, 0, 1) x | (A^T * B).row(2) | = (-m[2][1], Column[2][0], 0.0f)
  l = Vec3{ -transform.m_[2][1], transform.m_[2][0], 0.0f };
  absL = Absolute(l);
  projA = Dot(extentsA, absL);
  projB = Dot(extentsB, absL);
  s = abs(Dot(t, l)) - (projA + projB);
  if (CheckSeparation(out, s, l, 14))
    return false;

  return true;
}

bool SAT::CheckSeparation(FaceQuery& out, float separation, const Vec3& normal, unsigned index)
{
  // If separation distance is more than 0, means SAT detect that the convex shapes are not colliding
  if (separation > 0.0f)
    return true;

  // Update the separation distance to get the Minimum Translation Vector
  if (separation > out.maxSeparation_)
  {
    out.maxSeparation_ = separation;
    out.normal_ = normal;
    out.index_ = index;
  }
  return false;
}

bool SAT::CheckSeparation(EdgeQuery& out, float separation, const Vec3& normal, unsigned index)
{
  // If separation distance is more than 0, means SAT detect that the convex shapes are not colliding
  if (separation > 0.0f)
    return true;

  // Update the separation distance to get the Minimum Translation Vector
  if (separation > out.maxSeparation_)
  {
    out.maxSeparation_ = separation;
    out.normal_ = normal;
    out.index_ = index;
  }
  return false;
}
