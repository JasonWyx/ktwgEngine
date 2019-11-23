#pragma once

#include "vector3.h"
#include "matrix3.h"
#include "quaternion.h"

class Island;
struct Contact;
struct ContactInfo;
struct ContactState;

struct ContactInfoState
{
  void Set(ContactInfo* contactInfo, ContactState* contactState);

  Vec3  relativePosA_;					   // Vector from Center of Mass to contact position
  Vec3  relativePosB_;					   // Vector from Center of Mass to contact position
  Vec3  position_;                 // Position of the contact point
  Vec3  localPointA_;
  Vec3  localPointB_;
  Vec3  offsetA_;
  Vec3  offsetB_;
  float penetration_ = 0.0f; // Depth of penetration from collision
  float normalImpulse_ = 0.0f; // Accumulated normal impulse
  float tangentImpulse_[2] = {};   // Accumulated friction impulse
  float bias_ = 0.0f; // Restitution + baumgarte
  float normalMass_ = 0.0f; // Normal constraint mass
  float tangentMass_[2] = {};   // Tangent constraint mass
};

struct ContactState
{
  void Set(Contact* contact);

  ContactInfoState contactInfoStates_[8];
  Vec3             tangentVectors_[2];	  // Tangent vectors
  Vec3             normal_;				        // From A to B
  Vec3             centerA_;
  Vec3             centerB_;
  Vec3             localCenterA_;
  Vec3             localCenterB_;
  Vec3             scalarA_;
  Vec3             scalarB_;
  Matrix3          localInertiaA_;
  Matrix3          localInertiaB_;
  Matrix3          inertiaA_;
  Matrix3          inertiaB_;
  float            invMassA_ = 0.0f;
  float            invMassB_ = 0.0f;
  float            radiusA_ = 0.0f;
  float            radiusB_ = 0.0f;
  float            restitution_ = 0.0f;
  float            friction_ = 0.0f;
  int8_t           contactCount_ = 0;
  int32_t          indexA_ = -1;
  int32_t          indexB_ = -1;
  bool             useOffset_ = false;
};

class ContactSolver
{
  Vec3*         linearVelocities_;
  Vec3*         angularVelocities_;
  Vec3*         positions_;
  Quaternion* orientations_;
  Matrix3*      inverseInertias_;

  Island*       island_;
  ContactState* contactStates_;
  unsigned        contactCount_;
  unsigned        contactCapacity_;

public:
  ContactSolver(Island* island);
  ~ContactSolver();
  void ShutDown();
  void StoreImpulses();

  void WarmStart(float dt);
  void SolveVelocityConstraints();
  bool SolvePositionConstraints();
};
