#include "contactlistener.h"
#include "contact.h"

ContactListener::ContactListener(ContactDispatcher* dispatcher)
  : dispatcher_{ dispatcher }
{
}

void ContactListener::ContactStarted(Contact* contact)
{
  auto entityA = contact->pair_.colliders_[0]->GetRigidBody()->entity_;
  auto entityB = contact->pair_.colliders_[1]->GetRigidBody()->entity_;

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.method_ = BPCore::InteropMethods::COLLSTART;
  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);
  DispatchToDynamicParent(entityA, entityB, BPCore::InteropMethods::COLLSTART);
  DispatchToDynamicParent(entityB, entityA, BPCore::InteropMethods::COLLSTART);
}
