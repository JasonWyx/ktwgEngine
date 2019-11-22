/******************************************************************************/
/*!
\file       AABB3.h
\author     Koh Wen Lin
\par        Project: BPEngine
\brief      This is the interface file for AABB3.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

#include "vector3.h"
#include "raycast.h"
#include <algorithm>

/* Represent structure of a 3d Axis Align Bounding Box */
struct AABB3
{
  /* Utilities functions on AABB */
  void  Extend(float s);
  Vec3  Centroid()    const;
  float Height()      const;
  float Width()       const;
  float Depth()       const;
  int   LongestAxis() const;

  /* Query if point is in AABB */
  bool Contains(const Vec3& point) const;

  /* Query if another AABB is in AABB*/
  bool Contains(const AABB3& aabb) const;

  /* Closest point on AABB */
  Vec3 ClosestPointOnAABB(const Vec3& point) const;

  /* Compute the surface area of the AABB */
  float SurfaceArea();

  Vec3 m_Min; // The min point position of the aabb
  Vec3 m_Max; // The max point position of the aabb
};

// Test overlap between to AABB
inline bool AABB3toAABB3(const AABB3& lhs, const AABB3& rhs)
{
  if (lhs.m_Min.x_ > rhs.m_Max.x_ || rhs.m_Min.x_ > lhs.m_Max.x_)
    return false;

  if (lhs.m_Min.y_ > rhs.m_Max.y_ || rhs.m_Min.y_ > lhs.m_Max.y_)
    return false;

  if (lhs.m_Min.z_ > rhs.m_Max.z_ || rhs.m_Min.z_ > lhs.m_Max.z_)
    return false;

  return true;
}

// Test ray intersection with AABB, ray extended with radius = sphere
inline bool RayToAABB3(const RayCastInput& input, const AABB3& aabb, float radius = 0.0f)
{
  float tmin = -FLT_MAX; // Set to -FLT_MAX so we can test for time of first hit
  float tmax = 1.0f;     // Set this to the maximum our ray can travel

  // If radius is given, we give are doing a sphere to aabb check
  AABB3 extendedAABB = aabb;
  extendedAABB.Extend(radius);

  // Ray(t) = p + td
  const Vec3& p   = input.m_p;          // p is our starting point of the ray
  const Vec3& min = extendedAABB.m_Min; // grab our AABB informations
  const Vec3& max = extendedAABB.m_Max;

  // d is the direction the ray travel towards
  Vec3 d    = input.m_q - input.m_p;
  Vec3 absD = Absolute(d);

  // slabs are volume regions outside the bounding box
  // Check all three slabs in 3D space( x-slab, y-slab, z-slab )
  for (int i = 0; i < 3; ++i)
  {
    // Check each axis value of our ray direction. to test if the ray is parallel to the AABB
    if (absD[i] < FLT_EPSILON)
    {
      // Ray is parallel to slab
      if (p[i] < min[i] || p[i] > max[i])
        return false;
    }
    else
    {
      // Compute intersection t of ray with the near and far plane of slab
      float inv_d = 1.0f / d[i];
      float t0 = (min[i] - p[i]) * inv_d;
      float t1 = (max[i] - p[i]) * inv_d;

      // We always make t0 the intersection with near plane.
      // So we need to swap if our ray hits the far plane first
      if (t0 > t1)
        std::swap(t0, t1);

      // If t0 is more than our current tmin, we replace it
      tmin = t0 > tmin ? t0 : tmin;

      // Same goes to tmax, we bring down the time of intersection with far plane
      tmax = t1 < tmax ? t1 : tmin;

      // If the intersection intervals collapsed, no intersection occur
      if (tmin > tmax)
        return false;
    }
  }

  // If ray start inside box and ray intersects at time beyond our max time, no intersection occur
  // Intersection occur only withitn (0.0 - 1.0) and if ray does not start inside the box
  if (tmin < 0.0f || input.m_MaxT < tmin)
    return false;

  return true;
}

// Combine to AABB and return the result AABB
inline AABB3 Combine(const AABB3& lhs, const AABB3& rhs)
{
  AABB3 result;

  result.m_Min = MinVec(lhs.m_Min, rhs.m_Min);
  result.m_Max = MaxVec(lhs.m_Max, rhs.m_Max);

  return result;
}
