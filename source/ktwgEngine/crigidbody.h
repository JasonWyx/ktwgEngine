#pragma once

#include <vector>
#include "Component.h"
#include "rigidbodytype.h"

enum RBFlags
{
  RBF_NONE           = 0,
  RBF_AWAKE          = 1,
  RBF_ISLAND         = 1 << 1,
  RBF_FIXEDROTATIONX = 1 << 2,
  RBF_FIXEDROTATIONY = 1 << 3,
  RBF_FIXEDROTATIONZ = 1 << 4,
  RBF_IGNOREPHYSICS  = 1 << 5,
};

/* RigidBody Component class. This is used to manage any modification made to
   the rigid body's data.                                                     */
class CRigidBody : public Component
{
  template <typename T>
  using SharedPtr = std::shared_ptr<T>;

  template <typename T>
  using container_t = std::vector<T>;

public:
  CRigidBody(const SharedPtr<Entity>& owner, uint32_t id);
  ~CRigidBody();

  /* Override functions for abstract class */
  void Initialize() override;
  void Destroy()    override;

  void SetActive(bool active);

  // void AddForce(const Vec3& force);
  void SynchroniseRigidBody();

  /* Getters */
  const RBType&    GetBodyType() const;
  //const Vec3&     GetForce() const;
  //const Vec3&     GetTorque() const;
  //const Vec3&     GetLinearVelocity() const;
  //const Vec3&     GetAngularVelocity() const;
  float            GetMass() const;
  float            GetLinearDamping() const;
  float            GetAngularDamping() const;
  float            GetGravityScale() const;
  bool             GetUseGravity() const;
  bool             GetFreezeRotationX() const;
  bool             GetFreezeRotationY() const;
  bool             GetFreezeRotationZ() const;
  bool             GetIgnorePhysics() const;

  /* Setters */
  void SetBodyType(RBType type);
  //void SetForce(const Vec3& force);
  //void SetTorque(const Vec3& torque);
  //void SetLinearVelocity(const Vec3& velocity);
  //void SetAngularVelocity(const Vec3& velocity);
  void SetMass(float m);
  void SetLinearDamping(float value);
  void SetAngularDamping(float value);
  void SetGravityScale(float scale);
  void SetUseGravity(bool useGravity);
  void SetFreezeRotationX(bool freeze);
  void SetFreezeRotationY(bool freeze);
  void SetFreezeRotationZ(bool freeze);
  void SetIgnorePhysics(bool flag);

private:
  RBType m_RBType; // Specify body type of rigid body
  //Transform  ownerTransform_; // entity world transform
  //Transform  transform_;      // body world transform

  float m_Mass;               // Mass of the rigid body
  float m_InvMass;            // Inverse body mass

  //Matrix3 inertia_;          // Inertia about the local center position
  //Matrix3 invInertia_;       // Inverse inertia about the local center position
  //Matrix3 worldInvInertia_;  // Inverse inertia about the world center position

  //Vec3 force_;           // Force exerted on the rigid body
  //Vec3 torque_;          // Torque exerted on the rigid body
  //Vec3 linearVelocity_;  // Linear velocity on the rigid body
  //Vec3 angularVelocity_; // Angular velocity on the rigid body

  float m_LinearDamping;  // Linear damping applied on the rigid body
  float m_AngularDamping; // Angular damping applied on the rigid body
  float m_GravityScale;   // Gravity scalar applied on the rigid body

  bool m_UseGravity;      // Query if gravity should be applied

  //BPSweep sweep_;        // A barycentric formula to compute Time Of Impact collisions

  uint32_t m_IslandId;    // Index of body in the island
  uint32_t m_Flags;       // Body Flags
  float    m_SleepTime;   // Sleep time

  SharedPtr<void> m_World;     // The physics world this rigidbody belongs to

  // std::vector<SharedPtr<BoxCollider>> colliders_;

  bool m_Active;
};