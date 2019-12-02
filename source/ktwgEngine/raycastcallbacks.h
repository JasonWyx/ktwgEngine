#pragma once

#include "raycast.h"

class BroadPhase;

struct CastCallback
{
  bool QueryTriggers(int32_t id);

  virtual float Query(RayCastInput, int32_t) { return 0.0f; }
  virtual float Query(RayCastInput, int32_t, float) { return 0.0f; }

  const BroadPhase* broadPhase_; // Pointer to the broadphase object
  RayCastOutput*      output_;     // Pointer to the object to store our result
};

struct RayCastCallback : public CastCallback
{
  float Query(RayCastInput input, int32_t id) override;
};

//struct SphereCastCallback : public CastCallback
//{
//  float Query(RayCastInput input, int32_t id, float radius) override;
//};
