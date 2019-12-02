#include "raycastcallbacks.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "broadphase.h"

bool CastCallback::QueryTriggers(int32_t id)
{
  void* data = broadPhase_->GetUserData(id);
  BoxCollider* collider = static_cast<BoxCollider*>(data);

  return collider->GetIsTrigger();
}

float RayCastCallback::Query(RayCastInput input, int32_t id)
{
  // Retrieve the collider bounded by the bounding volume
  void* data = broadPhase_->GetUserData(id);
  BoxCollider* collider = static_cast<BoxCollider*>(data);

  // Don't check inactive collider
  if (!collider->GetActive())
    return input.m_MaxT;

  auto layerId = collider->GetRigidBody()->GetLayerId();
  auto layerBit = 1 << layerId;

  // Early out for the ray cast check for this collider if their layer is not included in layermask
  if (!(layerBit & input.m_LayerMask))
    return input.m_MaxT;


  // Perform raycast on the collider to test for intersection
  RayCastOutput out;
  auto hit = collider->RayCast(input, out);

  // If there's a hit, check if its the earliest based on time
  if (hit)
  {
    // Update the earlier intersection
    if (out.m_T < output_->m_T)
      *output_ = out;

    // Use this to clip our ray
    return output_->m_T;
  }

  // Return the maxt to continue our search for a better one if there are
  return input.m_MaxT;
}
