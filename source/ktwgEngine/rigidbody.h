#pragma once

#include <vector>
#include <memory>
#include "rigidbodytype.h"
#include "physicsconstants.h"
#include "vector3.h"
#include "matrix3.h"
#include "transform.h"
#include "sweep.h"

class Entity;
class BoxCollider;

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
  using container_t = std::vector<T>;

  template <typename T>
  using UniquePtr = std::unique_ptr<T>;

  friend class ContactListener;

public:
  RigidBody(Entity& entity);
  ~RigidBody();

  /* Apply force on the rigid body */
  void AddForce(const Vec3& force);

  /* Synchronise this rigid body with the owner entity */
  void SynchroniseRigidBody();

  /* Synchronise transformation of the object attached with this rigidbody */
  void SynchroniseTransform();

  /* Update the broadphase AABB tree */
  void SynchroniseProxies();

  /* Test if this body can collide with the other body */
  bool CanCollide(RigidBody* body);

  /* Getters */
  inline Entity*                                    GetOwner()           const { return m_Owner; }
  inline const container_t<UniquePtr<BoxCollider>>& GetColliders()       const { return m_Colliders; }
  inline const RBType&                              GetBodyType()        const { return m_Type; }
  inline const Transform&                           GetTransform()       const { return m_Transform; }
  inline const Vec3&                                GetForce()           const { return m_Force; }
  inline const Vec3&                                GetTorque()          const { return m_Torque; }
  inline const Vec3&                                GetLinearVelocity()  const { return m_LinearVelocity; }
  inline const Vec3&                                GetAngularVelocity() const { return m_AngularVelocity; }
  inline float                                      GetMass()            const { return m_Mass; }
  inline float                                      GetLinearDamping()   const { return m_LinearDamping; }
  inline float                                      GetAngularDamping()  const { return m_AngularDamping; }
  inline float                                      GetGravityScale()    const { return m_GravityScale; }
  uint32_t                                          GetLayerId()         const;
  inline bool                                       GetUseGravity()      const { return m_UseGravity; }
  inline bool                                       GetFreezeRotationX() const { return m_Flags & RBF_FIXEDROTATIONX; }
  inline bool                                       GetFreezeRotationY() const { return m_Flags & RBF_FIXEDROTATIONY; }
  inline bool                                       GetFreezeRotationZ() const { return m_Flags & RBF_FIXEDROTATIONZ; }
  inline bool                                       GetActive()          const { return m_Active; }
  inline bool                                       GetIgnorePhysics()   const { return m_Flags & RBF_IGNOREPHYSICS; }
  inline bool                                       IsAwake()            const { return m_Flags & RBF_AWAKE; }

  /* Setters */
  void SetBodyType(const RBType& type);
  void SetForce(const Vec3& force);
  void SetTorque(const Vec3& torque);
  void SetLinearVelocity(const Vec3& linearVelocity);
  void SetAngularVelocity(const Vec3& angularVelocity);
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
  void Set(RigidBody* rb);

private:
  BoxCollider* CreateCollider(uint32_t id);
  void         DestroyCollider(BoxCollider* collider);
  void         DestroyContacts();

  void ComputeInertia();

  friend class  CRigidBody;
  friend class  Physics;
  friend class  Island;
  friend class  ContactManager;
  friend class  ContactSolver;
  friend class  FCollider;
  friend struct ContactState;

  Entity* m_Owner;
  RBType m_Type;         // Specify body type of rigid body
  Transform m_Transform; // body world transform

  float m_Mass;    // Mass of the rigid body
  float m_InvMass; // Inverse body mass

  Matrix3 m_Inertia;         // Inertia about the local center position
  Matrix3 m_InvInertia;      // Inverse inertia about the local center position
  Matrix3 m_WorldInvInertia; // Inverse inertia about the world center position

  Vec3 m_Force;           // Force exerted on the rigid body
  Vec3 m_Torque;          // Torque exerted on the rigid body
  Vec3 m_LinearVelocity;  // Linear velocity on the rigid body
  Vec3 m_AngularVelocity; // Angular velocity on the rigid body

  float m_LinearDamping;  // Linear damping applied on the rigid body
  float m_AngularDamping; // Angular damping applied on the rigid body
  float m_GravityScale;   // Gravity scalar applied on the rigid body

  bool m_UseGravity;      // Query if gravity should be applied

  Sweep m_Sweep;        // A barycentric formula to compute Time Of Impact collisions

  uint32_t m_IslandId;    // Index of body in the island
  uint32_t m_Flags;       // Body Flags
  float    m_SleepTime;   // Sleep time

  container_t<UniquePtr<BoxCollider>> m_Colliders; // Vector of colliders/shapes

  bool m_Active;
};
