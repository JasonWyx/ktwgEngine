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
  toDispatch.type_ = CET_ONCOLLISIONENTER;

  dispatcher_->AddEvent(toDispatch);
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
  toDispatch.type_ = CET_ONCOLLISIONSTAY;

  dispatcher_->AddEvent(toDispatch);
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
  toDispatch.type_ = CET_ONCOLLISIONEXIT;

  dispatcher_->AddEvent(toDispatch);
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
  toDispatch.type_ = CET_ONTRIGGERENTER;

  dispatcher_->AddEvent(toDispatch);
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
  toDispatch.type_ = CET_ONTRIGGERSTAY;

  dispatcher_->AddEvent(toDispatch);
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
  toDispatch.type_ = CET_ONTRIGGEREXIT;

  dispatcher_->AddEvent(toDispatch);
}

void ContactListener::SetDispatcher(ContactDispatcher* dispatcher)
{
  dispatcher_ = dispatcher;
}
