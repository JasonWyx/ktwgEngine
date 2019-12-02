#pragma once

#include <assert.h>
#include "vector3.h"
#include "transform.h"

/* This class represents a 2D barycentric function to determine the resultant *
 * value of the function at time t.                                           */
struct Sweep
{
  inline Transform GetTransform(float t) const
  {
    auto center = (1.0f - t) * worldCenterOld_ + t * worldCenter_;
    auto orientation = (1.0f - t) * orientationOld_ + t * orientation_;
    orientation.Normalize();

    Transform transform;
    transform.SetRotation(orientation);
    transform.SetPosition(center - Multiply(orientation, localCenter_));

    return transform;
  }

  inline void Compute(float t)
  {
    assert(t > 0.0f && t < 1.0f);
    auto dt = (t - t_) / (1.0f / t_);
    worldCenter_ += dt * (worldCenter_ - worldCenterOld_);
    orientation_ += dt * (orientation_ - orientationOld_);
    orientation_.Normalize();
    t_ = t;
  }

  float t_ = 0.0f;

  Vec3 localCenter_;
  Vec3 worldCenter_;
  Vec3 worldCenterOld_;
  Quaternion orientation_;
  Quaternion orientationOld_;
};
