#include "collisionevent.h"
#include "entity.h"
#include "cbehaviour.h"
#include "cboxcollider.h"
#include "crigidbody.h"
#include "collision.h"
#include <vector>

void CollisionEvent::Dispatch()
{
  const std::vector<CBehaviour*> behsA = entityA_->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB_->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = infoA_.other_;
  collA.collider = infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = infoB_.other_;
  collB.collider = infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, type_);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collB, type_);
}
