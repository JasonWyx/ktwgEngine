#include "contact.h"
#include "contactlistener.h"
#include "fcollider.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "gjk.h"
#include "entity.h"

void Contact::Update(ContactListener* listener)
{
  // Get all necessary information from colliders
  // transform must be a world transformation
  BoxCollider* colliderA = pair_.colliders_[0];
  BoxCollider* colliderB = pair_.colliders_[1];
  RigidBody* bodyA = colliderA->GetRigidBody();
  RigidBody* bodyB = colliderB->GetRigidBody();

  bool isTriggerContact = colliderA->GetIsTrigger() || colliderB->GetIsTrigger();

  // Contact is a trigger contact, no need for generating contact, so we do GJK collision detection check
  if (isTriggerContact)
  {
    GJK::GJKOutput output = GJK::GJK(colliderA, colliderB);

    float radiiA = colliderA->GetRadius();
    float radiiB = colliderB->GetRadius();

    bool colliding = output.distance_ <= radiiA + radiiB;

    // Update the contact state.
    if (colliding == true)
      flags_ |= flags_ & eCFColliding ? eCFWasColliding : eCFColliding;
    else
    {
      if (flags_ & eCFColliding)
      {
        flags_ &= ~eCFColliding;
        flags_ |= eCFWasColliding;
      }
      else
        flags_ &= ~eCFWasColliding;
    }
  }
  else
  {
    // Copy out the old manifold
    Manifold oldManifold = manifold_;
    Vec3 oldTangent0 = oldManifold.tangentVectors_[0];
    Vec3 oldTangent1 = oldManifold.tangentVectors_[1];

    // Solve and generate a manifold
    SolveCollision();
    ComputeBasis(manifold_.normal_, &manifold_.tangentVectors_[0], &manifold_.tangentVectors_[1]);

    for (auto i = (int8_t)0; i < manifold_.contactCount_; ++i)
    {
      ContactInfo* contactInfo = manifold_.contactInfos_ + i;
      contactInfo->Initialize();

      for (auto j = 0; j < oldManifold.contactCount_; ++j)
      {
        ContactInfo* oldContactInfo = oldManifold.contactInfos_ + j;
        if (contactInfo->id_.key == oldContactInfo->id_.key)
        {
          contactInfo->normalImpulse_ = oldContactInfo->normalImpulse_;

          Vec3 friction = oldTangent0 * oldContactInfo->tangentImpulse_[0] + oldTangent1 * oldContactInfo->tangentImpulse_[1];
          contactInfo->tangentImpulse_[0] = Dot(friction, manifold_.tangentVectors_[0]);
          contactInfo->tangentImpulse_[1] = Dot(friction, manifold_.tangentVectors_[1]);
          break;
        }
      }
    }

    // If manifold has contact generated, It means there's a collision/no collision in this frame
    if (manifold_.contactCount_ > 0)
    {
      // If previous flag is Colliding, then current flag will be WasColliding because
      // the contact is already in colliding previous frame.
      // Whereas if the flag is not colliding, it means that the contact just begin its collision
      // So it is considered Colliding in this frame.
      flags_ |= flags_ & eCFColliding ? eCFWasColliding : eCFColliding;
    }
    else
    {
      // If no contact is generated in the manifold, it could mean the contact just ended collision
      // So if previous flag is Colliding, the current flag will not have Colliding set but only
      // wasColliding set to imply it had a past collision.
      // Whereas if flag is not in colliding anymore, we should reset the wasColliding flag to imply
      // there's no past collision this frame
      if (flags_ & eCFColliding)
      {
        flags_ &= ~eCFColliding;
        flags_ |= eCFWasColliding;
      }
      else
        flags_ &= ~eCFWasColliding;
    }
  }

  bool isColliding = flags_ & eCFColliding;
  bool wasColliding = flags_ & eCFWasColliding;

  // Wake the bodies associated with the shapes if the contact has began.
  if (isColliding != wasColliding)
  {
    bodyA->SetAwake(true);
    bodyB->SetAwake(true);
  }

  // Collision detection events information are passed to the event dispatcher
  if (listener)
  {
    if (isTriggerContact)
    {
      if (isColliding && !wasColliding)
        listener->TriggerStarted(this);
      else if (!isColliding && wasColliding)
        listener->TriggerEnded(this);
      else if (isColliding && wasColliding)
        listener->TriggerPersist(this);
    }
    else
    {
      if (isColliding && !wasColliding)
        listener->ContactStarted(this);
      else if (!isColliding && wasColliding)
        listener->ContactEnded(this);
      else if (isColliding && wasColliding)
        listener->ContactPersist(this);
    }
  }
}

void Contact::SolveCollision()
{
  // using CheckColliderFunctionTable = void(*)(Manifold& manifold, BoxCollider* colliderA, ICollider* colliderB);
  // 
  // // Collision function table
  // static const CheckColliderFunctionTable sCollideTable[eCTMax][eCTMax] =
  // {
  //   { NULL, NULL              , NULL                 , NULL                  , NULL                },
  //   { NULL, &CheckBoxToBox    , &CheckBoxToSphere    , &CheckBoxToCapsule    , &CheckBoxToMesh     },
  //   { NULL, &CheckSphereToBox , &CheckSphereToSphere , &CheckSphereToCapsule , &CheckSphereToMesh  },
  //   { NULL, &CheckCapsuleToBox, &CheckCapsuleToSphere, &CheckCapsuleToCapsule, &CheckCapsuleToMesh },
  //   { NULL, &CheckMeshToBox   , &CheckMeshToSphere   , &CheckMeshToCapsule   , &CheckMeshToMesh    }
  // };
  // 
  // manifold_.contactCount_ = 0;
  // 
  // auto typeA = pair_.colliders_[0]->GetType();
  // auto typeB = pair_.colliders_[1]->GetType();
  // 
  // // Call respective collision function
  // sCollideTable[typeA][typeB](manifold_, pair_.colliders_[0], pair_.colliders_[1]);
  CheckBoxToBox(manifold_, pair_.colliders_[0], pair_.colliders_[1]);
}

void Contact::DispatchEvent(ContactListener* listener)
{
  BoxCollider* colliderA = pair_.colliders_[0];
  BoxCollider* colliderB = pair_.colliders_[1];

  bool isTriggerContact = colliderA->GetIsTrigger() || colliderB->GetIsTrigger();
  bool isColliding = flags_ & eCFColliding;
  bool wasColliding = flags_ & eCFWasColliding;

  if (listener)
  {
    if (isTriggerContact)
    {
      if (isColliding && !wasColliding)
        listener->TriggerStarted(this);
      else if (!isColliding && wasColliding)
        listener->TriggerEnded(this);
      else if (isColliding && wasColliding)
        listener->TriggerPersist(this);
    }
    else
    {
      if (isColliding && !wasColliding)
        listener->ContactStarted(this);
      else if (!isColliding && wasColliding)
        listener->ContactEnded(this);
      else if (isColliding && wasColliding)
        listener->ContactPersist(this);
    }
  }
}
