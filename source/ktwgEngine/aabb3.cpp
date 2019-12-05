#include "aabb3.h"

void AABB3::Extend(float s)
{
  Vec3 r{ s, s, s };
  m_Min -= r;
  m_Max += r;
}

Vec3 AABB3::Centroid() const
{
  return 0.5f * (m_Min + m_Max);
}

float AABB3::Height() const
{
  return m_Max.y_ - m_Min.y_;
}

float AABB3::Width() const
{
  return m_Max.x_ - m_Min.x_;
}

float AABB3::Depth() const
{
  return m_Max.z_ - m_Min.z_;
}

int AABB3::LongestAxis() const
{
  // Return index of the largest axis
  // x is longest
  int longest = 0;
  float w = Width();
  float h = Height();
  float d = Depth();

  float max = w;

  // y is longer
  if (h > max)
  {
    max = h;
    longest = 1;
  }

  // z is longer
  if (d > max)
    longest = 2;

  return longest;
}

bool AABB3::Contains(const Vec3& point) const
{
  return m_Min.x_ <= point.x_ && m_Max.x_ >= point.x_ &&
         m_Min.y_ <= point.y_ && m_Max.y_ >= point.y_ &&
         m_Min.z_ <= point.z_ && m_Max.z_ >= point.z_;
}

bool AABB3::Contains(const AABB3& aabb) const
{
  return Contains(aabb.m_Min) && Contains(aabb.m_Max);
}

Vec3 AABB3::ClosestPointOnAABB(const Vec3& point) const
{
  Vec3 p;

  // For each axis, we clamp the points if its outside the aabb
  for (int i = 0; i < 3; ++i)
  {
    float v = point[i];
    if (v < m_Min[i])
      v = m_Min[i];

    if (v > m_Max[i])
      v = m_Max[i];

    p[i] = v;
  }

  return p;
}

float AABB3::SurfaceArea()
{
  float h = Height();
  float w = Width();
  float d = Depth();

  // Face area of height * width face
  float hw = h * w;

  // Face area of depth * width face
  float dw = d * w;

  // Face area of depth * height face
  float dh = d * h;

  float surface_area = 2.0f * (hw + dw + dh);

  return surface_area;
}
