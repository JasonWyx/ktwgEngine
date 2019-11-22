#include "sat.h"

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
