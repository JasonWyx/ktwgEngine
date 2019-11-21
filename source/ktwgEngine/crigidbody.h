#pragma once

#include <vector>
#include "Component.h"
#include "rigidbody.h"

/* RigidBody Component class. This is used to manage any modification made to
   the rigid body's data.                                                     */
class CRigidBody : public Component
{
  template <typename T>
  using container_t = std::vector<T>;

public:
  CRigidBody(Entity& owner, uint32_t id);
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
  RigidBody* m_Internal;
};