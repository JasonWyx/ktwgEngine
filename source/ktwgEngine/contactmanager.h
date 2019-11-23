#pragma once

#include "broadphase.h"
#include "customlist.h"
#include <memory>

/* Forward declaration */
class  ContactListener;
class  ContactDispatcher;
class  BoxCollider;
struct Contact;

/* This class manages all contact list in the physics pipeline. It contains *
 * a broad phase check and some collision rules to determine which contacts *
 * are to preceed to resolution stage.                                      */
class ContactManager
{
  template <typename T>
  using UniquePtr = std::unique_ptr<T>;

public:
  ContactManager();
  ~ContactManager();

  /* Generate contact list to resolve colliders' contact points */
  void GenerateContactPair();

  /* Add a contact pair into contact list */
  void AddContactPair(BoxCollider* colliderA, BoxCollider* colliderB);

  /* Update the contact list after constraint checks */
  void UpdateContacts();

  /* Reset our contact manager */
  void Reset();

  /* Clean Up our contact manager */
  void CleanUp();

  /* Clear the contact list */
  void ClearContacts();

  /* Clear the contact list */
  void ClearDispatcher();

  /* Reset our broadphase data, used for scene changing when no destruction happens but just want to reset it */
  void ResetBroadPhase();

  /* Ensure all contact resources are removed and released */
  void RemoveContact(Contact* contact);

  /* Dispatches all collision events */
  void DispatchAllCollisionEvents();

  BroadPhase                   broadPhase_;        // Broad phase algorithm
  UniquePtr<ContactListener>   contactListener_;   // A listener which listens for contact information
  UniquePtr<ContactDispatcher> contactDispatcher_; // A dispatcher which dispatches contact information
  List<Contact>                contactList_;       // The list of contacts generated

};
