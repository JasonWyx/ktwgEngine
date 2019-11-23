#pragma once

/* Forward declaration */
struct Contact;
struct CollisionInfo;
class  ContactDispatcher;

class ContactListener
{
public:
  ContactListener(ContactDispatcher* dispatcher);

  void ContactStarted(Contact* contact);
  void ContactPersist(Contact* contact);
  void ContactEnded(Contact* contact);
  void TriggerStarted(Contact* contact);
  void TriggerPersist(Contact* contact);
  void TriggerEnded(Contact* contact);

  //void DispatchToDynamicParent(Entity& entityA, Entity& entityB, BPCore::InteropMethods methods);

  void SetDispatcher(ContactDispatcher* dispatcher);

private:
  ContactDispatcher* dispatcher_;
};
