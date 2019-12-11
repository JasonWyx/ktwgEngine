#pragma once

#include <vector>
#include "Component.h"
#include "rigidbody.h"

/* RigidBody Component class. This is used to manage any modification made to
   the rigid body's data.                                                     */
class CRigidBody final : public Component
{
  template <typename T>
  using container_t = std::vector<T>;

public:
  CRigidBody(Entity& owner, uint32_t id);
  ~CRigidBody();

  /* Override functions for abstract class */
  void Initialize() override;
  void Destroy()    override;

  void SetActive(bool active) override;

  void AddForce(const Vec3& force);
  void SynchroniseRigidBody();

  /* Getters */
  RigidBody*    GetInternal()        const;
  const RBType& GetBodyType()        const;
  const Vec3&   GetForce()           const;
  const Vec3&   GetTorque()          const;
  const Vec3&   GetLinearVelocity()  const;
  const Vec3&   GetAngularVelocity() const;
  float         GetMass()            const;
  float         GetLinearDamping()   const;
  float         GetAngularDamping()  const;
  float         GetGravityScale()    const;
  bool          GetUseGravity()      const;
  bool          GetFreezeRotationX() const;
  bool          GetFreezeRotationY() const;
  bool          GetFreezeRotationZ() const;
  bool          GetIgnorePhysics()   const;

  /* Setters */
  void SetBodyType(RBType type);
  void SetForce(const Vec3& force);
  void SetTorque(const Vec3& torque);
  void SetLinearVelocity(const Vec3& linearVelocity);
  void SetAngularVelocity(const Vec3& angularVelocity);
  void SetMass(float m);
  void SetLinearDamping(float value);
  void SetAngularDamping(float value);
  void SetGravityScale(float scale);
  void SetUseGravity(bool useGravity);
  void SetFreezeRotationX(bool freeze);
  void SetFreezeRotationY(bool freeze);
  void SetFreezeRotationZ(bool freeze);
  void SetIgnorePhysics(bool flag);

  void Set(Component* comp) override;

  // Network
  void GhostPropertyReadStream(BitStream& stream) override;
  void GhostPropertyWriteStream(BitStream& stream) override;
  void GhostPropertyReplicateFromStream(BitStream& stream) override;

  void RegisterAsGhostProperty(GhostObject* ghostObject, NetAuthority netAuthority) override;

private:
  RigidBody* m_Internal;
};

// Type T = Component Type e.g ComponentGhostProperty<CRenderable>
template <>
class GhostPropertyComponent<CRigidBody, typename std::enable_if<std::is_base_of<Component, CRigidBody>::value>::type> : public GhostProperty
{
public:
  GhostPropertyComponent(CRigidBody* component, NetAuthority authority)
    : GhostProperty{ authority }
    , m_Component{ component }
  {

  }

  void WriteStream(BitStream& stream) override
  {
    m_Component->GhostPropertyWriteStream(stream);
  }

  void ReadStream(BitStream& stream) override
  {
    m_Component->GhostPropertyReadStream(stream);
  }

  bool IsPropertyChanged() const override
  {
    Vec3 linearVel = m_Component->GetLinearVelocity();
    return !(linearVel == m_PrevLinearVel);
  }

  void Sync() override
  {
    m_PrevLinearVel = m_Component->GetLinearVelocity();
  }

private:
  Vec3 m_PrevLinearVel;
  CRigidBody* m_Component;
};
