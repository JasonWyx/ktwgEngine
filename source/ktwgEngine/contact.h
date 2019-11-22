#pragma once

#include "vector3.h"
#include "collisionfunctions.h"

struct ContactFeature;
struct ContactEdge;
struct ContactPair;
struct Contact;
struct Manifold;
class BoxCollider;
class ContactListener;
class RigidBody;

/* Features intersecting to form the contact point */
struct ContactFeature
{
  uint8_t inReference_;
  uint8_t outReference_;
  uint8_t inIncident_;
  uint8_t outIncident_;
};

union ContactId
{
  ContactFeature cf_;
  uint32_t key;         // Used to compare contact ids
};

/* Represents a contact edge */
struct ContactEdge
{
  BoxCollider* other_; // The other collider in contact
  Contact* contact_;   // The contact

  ContactEdge* next_;
  ContactEdge* prev_;
};

/* Represents a pair of contact. */
struct ContactPair
{
  BoxCollider* colliders_[2]; // The two colliders in as a contact pair
  ContactEdge  edges_[2];     // The two edges linking the two colliders
};

/* Represents a contact between two colliders */
struct ContactInfo
{
  void Initialize()
  {
    tangentImpulse_[0] = 0.0f;
    tangentImpulse_[1] = 0.0f;
    normalImpulse_     = 0.0f;
  }

  Vec3  position_          = Vec3{}; // World position of contact
  Vec3  localPointA_       = Vec3{};
  Vec3  localPointB_       = Vec3{};
  Vec3  offsetA_           = Vec3{};
  Vec3  offsetB_           = Vec3{};
  float penetration_       = 0.0f;   // Penetration depth from collision
  float normalImpulse_     = 0.0f;   // Accumulated normal impulse
  float tangentImpulse_[2] = {};     // Accumulated friction impulse
  float bias_              = 0.0f;   // Restitution + baumgarte
  float tangentMass_[2]    = {};     // Normal constraint mass
  ContactId id_;                     // to uniquely identify a contact point between two shape
};

/* Represent a group of contact points sharing the similar contact normals */
struct Manifold
{
  BoxCollider* colliderA_;
  BoxCollider* colliderB_;

  Vec3 normal_;                // normal from A to B
  Vec3 tangentVectors_[2];     // Tangent vectors

  ContactInfo contactInfos_[8];
  int8_t      contactCount_;

  // Check if this manifold is a trigger collision
  bool trigger_;
  bool useOffset_ = false;

  // Used for Manifold doubly linked list
  Manifold* next_;
  Manifold* prev_;
};

/* Represents a contact point */
struct Contact
{
  void Update(ContactListener* listener);
  void SolveCollision();
  void DispatchEvent(ContactListener* listener);

  friend class Physics;

  enum ContactFlag
  {
    eCFColliding = 0x1, // Set this when two contact collides
    eCFWasColliding = 0x2, // Set this when two object end collision
    eCFIslandFlag = 0x4  // Set this to mark that contact is in an island
  };

  // Contains a pair of colliders in contact and a pair of edge owned by the colliders representing linked contacts via edge
  ContactPair pair_;

  // Contact friction and restitution used for physics simulation between the two contact pair
  float friction_;
  float restitution_;

  // Manifold contains contact information used to resolve our contacts
  Manifold manifold_;

  // Contact flags
  uint32_t flags_;

  // Used to build a doubly linked list
  Contact* next_;
  Contact* prev_;
};
