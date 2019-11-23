#include "contactmanager.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "fcollider.h"
#include "contact.h"
#include "contactlistener.h"
#include "contactdispatcher.h"
#include "gjk.h"

ContactManager::ContactManager()
  : broadPhase_{ this }, 
    contactListener_{ nullptr },
    contactDispatcher_{ nullptr },
    contactList_{}
{
  contactDispatcher_ = std::make_unique<ContactDispatcher>();
  contactListener_ = std::make_unique<ContactListener>(contactDispatcher_.get());
}

ContactManager::~ContactManager()
{
}

void ContactManager::GenerateContactPair()
{
  broadPhase_.GeneratePairs();
}

void ContactManager::AddContactPair(BoxCollider* colliderA, BoxCollider* colliderB)
{
  RigidBody* bodyA = colliderA->GetRigidBody();
  RigidBody* bodyB = colliderB->GetRigidBody();

  if (!bodyA->CanCollide(bodyB))
    return;

  // If either 1 collider is inactive, we ignored 
  if (!colliderA->GetActive() || !colliderB->GetActive())
    return;

  // Search through edge lists to see if there is already a collision edge between the pair
  const List<ContactEdge>& edgeListA = colliderA->GetInternal()->m_ContactEdgeList;

  ContactEdge* edge = edgeListA.head_;
  while (edge)
  {
    // Contact edge already exist between the pair, dont need to duplicate
    if (edge->other_ == colliderB)
      return;

    edge = edge->next_;
  }

  // Create new contact
  Contact* contact = new Contact();
  contact->pair_.colliders_[0] = colliderA;
  contact->pair_.colliders_[1] = colliderB;
  contact->manifold_.colliderA_ = colliderA;
  contact->manifold_.colliderB_ = colliderB;
  contact->manifold_.trigger_ = colliderA->GetIsTrigger() || colliderB->GetIsTrigger();
  contact->friction_ = std::sqrt(colliderA->GetFriction() * colliderB->GetFriction());
  float resA = colliderA->GetRestitution();
  float resB = colliderB->GetRestitution();
  contact->restitution_ = resA > resB ? resA : resB;
  contact->manifold_.contactCount_ = 0;
  contact->flags_ = 0;

  contactList_.PushFront(contact);

  // Connect A
  contact->pair_.edges_[0].contact_ = contact;
  contact->pair_.edges_[0].other_ = colliderB;

  colliderA->AddContactEdge(&contact->pair_.edges_[0]);

  // Connect B
  contact->pair_.edges_[1].contact_ = contact;
  contact->pair_.edges_[1].other_ = colliderA;

  colliderB->AddContactEdge(&contact->pair_.edges_[1]);

  if (!colliderA->GetIsTrigger() && !colliderB->GetIsTrigger())
  {
    bodyA->SetAwake(true);
    bodyB->SetAwake(true);
  }
}

void ContactManager::UpdateContacts()
{
  // Loop through all contacts
  Contact* contact = contactList_.head_;

  while (contact)
  {
    // Retrieve the pair of colliders which are in contact chosen from broadphase
    ContactPair* pair = &contact->pair_;

    BoxCollider* colliderA = pair->colliders_[0];
    RigidBody* bodyA = colliderA->GetRigidBody();

    BoxCollider* colliderB = pair->colliders_[1];
    RigidBody* bodyB = colliderB->GetRigidBody();

    contact->flags_ &= ~Contact::CF_ISLANDFLAG;

    // If both bodies will never be collided, remove from the contact list
    if (!bodyA->CanCollide(bodyB))
    {
      auto prune = contact;
      contact = contact->next_;
      RemoveContact(prune);
      continue;
    }

    bool isStaticA = bodyA->GetBodyType() == RBT_STATIC;
    bool isStaticB = bodyB->GetBodyType() == RBT_STATIC;

    // At least one body must be dynamic/kinematic
    if (isStaticA && isStaticB)
    {
      contact = contact->next_;
      continue;
    }

    // If there's a trigger collider, we need to make sure to wake the non trigger collider up for collision test
    if (colliderA->GetIsTrigger() || colliderB->GetIsTrigger())
    {
      GJK::GJKOutput output = GJK::GJK(colliderA, colliderB);

      float radiiA = colliderA->GetRadius();
      float radiiB = colliderB->GetRadius();

      // Update the contact state.
      if (output.distance_ <= radiiA + radiiB)
      {
        bodyA->SetAwake(true);
        bodyB->SetAwake(true);
      }
    }

    bool isAwakeA = bodyA->IsAwake();
    bool isAwakeB = bodyB->IsAwake();

    // At least one body must be awake
    if (!isAwakeA && !isAwakeB)
    {
      // Still need to dispatch collision event while sleeping
      contact->DispatchEvent(contactListener_.get());
      contact = contact->next_;
      continue;
    }

    // Broadphase overlap test
    int32_t indexA = colliderA->GetInternal()->GetBroadphaseId();
    int32_t indexB = colliderB->GetInternal()->GetBroadphaseId();

    // Check if contact should persist
    if (!broadPhase_.TestOverlap(indexA, indexB))
    {
      Contact* prune = contact;
      contact = contact->next_;
      RemoveContact(prune);
      continue;
    }

    // Update persisting contact as it passes all constraint test, run it through collision detection
    // algorithm and generate manifold for each contact
    contact->Update(contactListener_.get());
    contact = contact->next_;
  }
}

void ContactManager::Reset()
{
  ClearContacts();
  ResetBroadPhase();
}

void ContactManager::CleanUp()
{
  ClearContacts();
  ResetBroadPhase();
}

void ContactManager::ClearContacts()
{
  Contact* current = contactList_.head_;
  while (current)
  {
    auto remove = current;
    current = current->next_;
    RemoveContact(remove);
  }
  contactList_.Clear();
}

void ContactManager::ClearDispatcher()
{
  contactDispatcher_->Clear();
}

void ContactManager::ResetBroadPhase()
{
  broadPhase_.Reset();
}

void ContactManager::RemoveContact(Contact* contact)
{
  ContactPair* pair = &contact->pair_;

  BoxCollider* colliderA = pair->colliders_[0];
  BoxCollider* colliderB = pair->colliders_[1];

  // Remove the contact edges list from both colliders
  colliderA->GetInternal()->m_ContactEdgeList.Remove(&pair->edges_[0]);
  colliderB->GetInternal()->m_ContactEdgeList.Remove(&pair->edges_[1]);

  // Remove contact from list
  contactList_.Remove(contact);

  // Release the contact resource
  delete contact;
}

void ContactManager::DispatchAllCollisionEvents()
{
  contactDispatcher_->DispatchAll();
}
