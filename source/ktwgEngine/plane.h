#pragma once

#include "vector3.h"
//#include "Utilities/Math/Transform.h"

/* Represent a plane with normal and distance to origin */
struct Plane
{
  Plane() {}

  /* Conver a normal and a point on the plane to an equation */
  Plane(const Vec3& normal, const Vec3& point)
  {
    normal_ = normal;
    d_      = Dot(normal, point);
  }

  // Plane Equation : normal . x = d
  Vec3  normal_;
  float d_;
};

//inline Plane Multiply(const Transform& transform, const Plane& plane)
//{
//  Plane result;
//
//  result.normal_ = Multiply(transform.GetRotation(), plane.normal_);
//  result.d_ = plane.d_ + Dot(result.normal_, transform.GetPosition());
//
//  return result;
//}

inline float Distance(const Vec3& p, const Plane& plane)
{
  // Signed distance of point from plane
  return Dot(plane.normal_, p) - plane.d_;
}

inline Vec3 ClosestPointOnPlane(const Vec3& p, const Plane& plane)
{
  auto t = Distance(p, plane);
  return p - t * plane.normal_;
}
