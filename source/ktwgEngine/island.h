#pragma once

#include <vector>
#include "matrix3.h"
#include "vector3.h"
#include "quaternion.h"

/* Forward declarations */
class RigidBody;
struct Contact;
struct ContactState;

/* Island flags enum */
enum IslandFlags
{
  IF_WARMSTART = 0x0001,
  IF_SLEEP = 0x0002
};

/* This class represents a simulation island used to optimise our physics   *
 * world simulation by calculating rigid bodies which are awake only. Awake *
 * means that the object is currently still affected by physics forces. If  *
 * the motions are under a certain tolerance level, we treat the bodies as  *
 * asleep.                                                                  */
class Island
{
public:
  Island(unsigned bodyCap, unsigned contactCap);
  ~Island();

  /* Clear the bodies in this island */
  void Clear();

  /* Add a rigid body into this island */
  void AddBody(RigidBody* body);

  /* Add a contact body into this island */
  void AddContact(Contact* contact);

  /* Simulate the physics on the rigid bodies in this island */
  void Solve(const Vec3& external_force, float dt, unsigned vel_iteration, unsigned pos_iteration, unsigned flag);

private:
  friend class Physics;
  friend class ContactSolver;

  std::vector<RigidBody*> bodies_;       // Bodies formed in this island
  unsigned                bodyCapacity_; // The worst case capacity of an island
  unsigned                bodyCount_;    // Current body count in this island

  std::vector<Contact*> contacts_;
  unsigned              contactCapacity_;
  unsigned              contactCount_;


  // The motions computed are stored in this containers and are synced back
  // to the respective rigid bodies at the end.
  Vec3*       positions_;
  Vec3*       linearVelocities_;
  Vec3*       angularVelocities_;
  Quaternion* orientations_;
  Matrix3*    inverseInertias_;

  bool allowSleep_;
};
