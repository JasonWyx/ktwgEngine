#include "contactlistener.h"
#include "contactdispatcher.h"
#include "contact.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "entity.h"
#include "collisioninfo.h"
#include "collisionevent.h"

#include "collision.h"
#include "entity.h"
#include "cbehaviour.h"
#include "crigidbody.h"
#include "cboxcollider.h"


ContactListener::ContactListener(ContactDispatcher* dispatcher)
  : dispatcher_{ dispatcher }
{
}

void ContactListener::ContactStarted(Contact* contact)
{
  Entity* entityA = contact->pair_.colliders_[0]->GetRigidBody()->m_Owner;
  Entity* entityB = contact->pair_.colliders_[1]->GetRigidBody()->m_Owner;

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);

  const std::vector<CBehaviour*> behsA = entityA->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = toDispatch.infoA_.other_;
  collA.collider = toDispatch.infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = toDispatch.infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &toDispatch.infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = toDispatch.infoB_.other_;
  collB.collider = toDispatch.infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = toDispatch.infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &toDispatch.infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, CET_ONCOLLISIONENTER);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collA, CET_ONCOLLISIONENTER);
}

void ContactListener::ContactPersist(Contact* contact)
{
  Entity* entityA = contact->pair_.colliders_[0]->GetRigidBody()->GetOwner();
  Entity* entityB = contact->pair_.colliders_[1]->GetRigidBody()->GetOwner();

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);

  const std::vector<CBehaviour*> behsA = entityA->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = toDispatch.infoA_.other_;
  collA.collider = toDispatch.infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = toDispatch.infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &toDispatch.infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = toDispatch.infoB_.other_;
  collB.collider = toDispatch.infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = toDispatch.infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &toDispatch.infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, CET_ONCOLLISIONSTAY);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collA, CET_ONCOLLISIONSTAY);
}

void ContactListener::ContactEnded(Contact* contact)
{
  Entity* entityA = contact->pair_.colliders_[0]->GetRigidBody()->GetOwner();
  Entity* entityB = contact->pair_.colliders_[1]->GetRigidBody()->GetOwner();

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);

  const std::vector<CBehaviour*> behsA = entityA->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = toDispatch.infoA_.other_;
  collA.collider = toDispatch.infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = toDispatch.infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &toDispatch.infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = toDispatch.infoB_.other_;
  collB.collider = toDispatch.infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = toDispatch.infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &toDispatch.infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, CET_ONCOLLISIONEXIT);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collA, CET_ONCOLLISIONEXIT);
}

void ContactListener::TriggerStarted(Contact* contact)
{
  Entity* entityA = contact->pair_.colliders_[0]->GetRigidBody()->GetOwner();
  Entity* entityB = contact->pair_.colliders_[1]->GetRigidBody()->GetOwner();

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);

  const std::vector<CBehaviour*> behsA = entityA->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = toDispatch.infoA_.other_;
  collA.collider = toDispatch.infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = toDispatch.infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &toDispatch.infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = toDispatch.infoB_.other_;
  collB.collider = toDispatch.infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = toDispatch.infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &toDispatch.infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, CET_ONTRIGGERENTER);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collA, CET_ONTRIGGERENTER);
}

void ContactListener::TriggerPersist(Contact* contact)
{
  Entity* entityA = contact->pair_.colliders_[0]->GetRigidBody()->GetOwner();
  Entity* entityB = contact->pair_.colliders_[1]->GetRigidBody()->GetOwner();

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);

  const std::vector<CBehaviour*> behsA = entityA->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = toDispatch.infoA_.other_;
  collA.collider = toDispatch.infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = toDispatch.infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &toDispatch.infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = toDispatch.infoB_.other_;
  collB.collider = toDispatch.infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = toDispatch.infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &toDispatch.infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, CET_ONTRIGGERSTAY);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collA, CET_ONTRIGGERSTAY);
}

void ContactListener::TriggerEnded(Contact* contact)
{
  Entity* entityA = contact->pair_.colliders_[0]->GetRigidBody()->GetOwner();
  Entity* entityB = contact->pair_.colliders_[1]->GetRigidBody()->GetOwner();

  CollisionInfo infoA, infoB;
  infoA.other_ = entityB;
  infoB.other_ = entityA;

  CollisionEvent toDispatch;

  toDispatch.entityA_ = entityA;
  toDispatch.entityB_ = entityB;
  toDispatch.infoA_ = infoA;
  toDispatch.infoB_ = infoB;

  dispatcher_->AddEvent(toDispatch);

  const std::vector<CBehaviour*> behsA = entityA->GetAllComponentsOfType<CBehaviour>();
  const std::vector<CBehaviour*> behsB = entityB->GetAllComponentsOfType<CBehaviour>();

  Collision collA;
  collA.entity = toDispatch.infoA_.other_;
  collA.collider = toDispatch.infoA_.other_->GetComponent<CBoxCollider>();
  collA.rigidBody = toDispatch.infoA_.other_->GetComponent<CRigidBody>();
  collA.transform = &toDispatch.infoA_.other_->GetTransform();

  Collision collB;
  collB.entity = toDispatch.infoB_.other_;
  collB.collider = toDispatch.infoB_.other_->GetComponent<CBoxCollider>();
  collB.rigidBody = toDispatch.infoB_.other_->GetComponent<CRigidBody>();
  collB.transform = &toDispatch.infoB_.other_->GetTransform();

  for (auto& elem : behsA)
    elem->DispatchCollisionEvent(collA, CET_ONTRIGGEREXIT);

  for (auto& elem : behsB)
    elem->DispatchCollisionEvent(collA, CET_ONTRIGGEREXIT);
}

void ContactListener::SetDispatcher(ContactDispatcher* dispatcher)
{
  dispatcher_ = dispatcher;
}
