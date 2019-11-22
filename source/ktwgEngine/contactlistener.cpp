#include "contactlistener.h"
#include "contact.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "entity.h"

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
}

void ContactListener::ContactPersist(Contact* contact)
{
}

void ContactListener::ContactEnded(Contact* contact)
{
}

void ContactListener::TriggerStarted(Contact* contact)
{
}

void ContactListener::TriggerPersist(Contact* contact)
{
}

void ContactListener::TriggerEnded(Contact* contact)
{
}

void ContactListener::SetDispatcher(ContactDispatcher* dispatcher)
{
}
