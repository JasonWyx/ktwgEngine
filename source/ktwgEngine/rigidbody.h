#pragma once

#include <vector>
#include <memory>
#include "rigidbodytype.h"
#include "physicsconstants.h"

class Entity;

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

class RigidBody
{
  template <typename T>
  using SharedPtr = std::shared_ptr<T>;

  template <typename T>
  using container_t = std::vector<T>;

public:
  RigidBody(const Entity& entity);
  ~RigidBody();

  /* Apply force on the rigid body */
  //void AddForce(const Vec3& force) override;

  /* Synchronise this rigid body with the owner entity */
  void SynchroniseRigidBody();

  /* Synchronise transformation of the object attached with this rigidbody */
  void SynchroniseTransform();

  /* Update the broadphase AABB tree */
  void SynchroniseProxies();

  /* Test if this body can collide with the other body */
  bool CanCollide(RigidBody* body);

  /* Getters */
  //SharedPtr<Physics>             GetWorld() const;
  //const std::vector<SharedPtr<BoxCollider>>& GetColliders() const override;
  inline const RBType& GetBodyType() const { return m_Type; }
  //const Transform&               GetTransform() const override;
  //const Vec3&                    GetForce() const override;
  //const Vec3&                    GetTorque() const override;
  //const Vec3&                    GetLinearVelocity() const override;
  //const Vec3&                    GetAngularVelocity() const override;
  inline float GetMass()            const { return m_Mass; }
  inline float GetLinearDamping()   const { return m_LinearDamping; }
  inline float GetAngularDamping()  const { return m_AngularDamping; }
  inline float GetGravityScale()    const { return m_GravityScale; }
  inline bool  GetUseGravity()      const { return m_UseGravity; }
  inline bool  GetFreezeRotationX() const { return m_Flags & RBF_FIXEDROTATIONX; }
  inline bool  GetFreezeRotationY() const { return m_Flags & RBF_FIXEDROTATIONY; }
  inline bool  GetFreezeRotationZ() const { return m_Flags & RBF_FIXEDROTATIONZ; }
  inline bool  GetActive()          const { return m_Active; }
  inline bool  GetIgnorePhysics()   const { return m_Flags & RBF_IGNOREPHYSICS; }
  inline bool  IsAwake()            const { return m_Flags & RBF_AWAKE; }

  /* Setters */
  void SetBodyType(const RBType& type);
  //void SetForce(const Vec3& force)    override;
  //void SetTorque(const Vec3& torque)   override;
  //void SetLinearVelocity(const Vec3& velocity) override;
  //void SetAngularVelocity(const Vec3& velocity) override;
  void SetMass(float m);
  void SetLinearDamping(float linearDamping);
  void SetAngularDamping(float angularDamping);
  void SetGravityScale(float gravityScale);
  void SetUseGravity(bool useGravity);
  void SetFreezeRotationX(bool freeze);
  void SetFreezeRotationY(bool freeze);
  void SetFreezeRotationZ(bool freeze);
  void SetActive(bool active);
  void SetAwake(bool flag);
  void SetIgnorePhysics(bool flag);

private:
  // SharedPtr<BoxCollider> CreateCollider(ColliderType type, size_t id, uint32_t meshId = 0) override;
  //void       DestroyCollider(ICollider* collider)                               override;
  void DestroyContacts();
  //void       DeleteCollider(ICollider* collider);

  void ComputeInertia();

  friend class  CRigidBody;
  friend class  Physics;
  friend class  Island;
  friend class  ContactManager;
  friend class  ContactSolver;
  friend class  FCollider;
  friend struct ContactState;

  Entity const* m_Entity;
  RBType m_Type;       // Specify body type of rigid body
  //Transform  ownerTransform_; // entity world transform
  //Transform  transform_;      // body world transform

  float m_Mass;               // Mass of the rigid body
  float m_InvMass;            // Inverse body mass

  // Matrix3 inertia_;          // Inertia about the local center position
  // Matrix3 invInertia_;       // Inverse inertia about the local center position
  // Matrix3 worldInvInertia_;  // Inverse inertia about the world center position

  // Vec3 force_;           // Force exerted on the rigid body
  // Vec3 torque_;          // Torque exerted on the rigid body
  // Vec3 linearVelocity_;  // Linear velocity on the rigid body
  // Vec3 angularVelocity_; // Angular velocity on the rigid body

  float m_LinearDamping;  // Linear damping applied on the rigid body
  float m_AngularDamping; // Angular damping applied on the rigid body
  float m_GravityScale;   // Gravity scalar applied on the rigid body

  bool m_UseGravity;      // Query if gravity should be applied

  // Sweep sweep_;        // A barycentric formula to compute Time Of Impact collisions

  uint32_t m_IslandId;    // Index of body in the island
  uint32_t m_Flags;       // Body Flags
  float    m_SleepTime;   // Sleep time

  //std::vector<ICollider*> colliders_; // Vector of colliders/shapes

  bool m_Active;
};
