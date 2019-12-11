#include "crigidbody.h"
#include "physics.h"
#include "entity.h"

CRigidBody::CRigidBody(Entity& entity, uint32_t id)
  : Component{ typeid(CRigidBody), entity, id }, m_Internal{ nullptr }
{
  SetType(CT_RIGIDBODY);
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Initialize()
{
  m_Internal = Physics::GetInstance().CreateRigidBody(*GetOwner());
}

void CRigidBody::Destroy()
{
  Entity* entity = GetOwner();

  // copy list of components
  const std::vector<Component*>& components = entity->GetComponents();

  // delete all colliders
  for (Component* const & c : components)
  {
    if (c->GetType() == CT_BOXCOLLIDER)
      entity->RemoveComponent(c);
  }

  if (m_Internal)
  {
    Physics::GetInstance().DestroyRigidBody(m_Internal);
    m_Internal = nullptr;
  }
}

void CRigidBody::SetActive(bool active)
{
  Component::SetActive(active);

  assert(m_Internal);

  m_Internal->SetActive(active);
}

void CRigidBody::Set(Component* comp)
{
  Component::Set(comp);

  assert(m_Internal);

  CRigidBody* rb = static_cast<CRigidBody*>(comp);
  m_Internal->Set(rb->m_Internal);
}

void CRigidBody::GhostPropertyReadStream(BitStream & stream)
{
  ComponentID dummyCi;
  stream >> dummyCi;

  ComponentType dummyType;
  stream >> dummyType;

  Vec3 linearVelocity;
  stream >> linearVelocity.x_ >> linearVelocity.y_ >> linearVelocity.z_;
  Vec3 angularVelocity;
  stream >> angularVelocity.x_ >> angularVelocity.y_ >> angularVelocity.z_;
  uint8_t bodyType;
  stream >> bodyType;
  float mass;
  stream >> mass;
  float linearDamping;
  stream >> linearDamping;
  float angularDamping;
  stream >> angularDamping;
  float gravityScale;
  stream >> gravityScale;
  bool useGravity;
  stream >> useGravity;
  stream >> m_Internal->m_Flags;

  SetLinearVelocity(linearVelocity);
  SetAngularVelocity(angularVelocity);
}

void CRigidBody::GhostPropertyWriteStream(BitStream & stream)
{
  // ALWAYS PREFIX WITH CLASSID FOR COMPONENT AND COMPONENTTYPE DON'T NEED TO READ THIS BACK
  stream << CI_Component;
  stream << GetType();

  Vec3 linearVelocity = GetLinearVelocity();
  stream << linearVelocity.x_ << linearVelocity.y_ << linearVelocity.z_;
  Vec3 angularVelocity = GetAngularVelocity();
  stream << angularVelocity.x_ << angularVelocity.y_ << angularVelocity.z_;
  stream << (uint8_t)GetBodyType();
  stream << GetMass();
  stream << GetLinearDamping();
  stream << GetAngularDamping();
  stream << GetGravityScale();
  stream << GetUseGravity();
  stream << m_Internal->m_Flags;
}

void CRigidBody::GhostPropertyReplicateFromStream(BitStream & stream)
{
  Vec3 linearVelocity;
  stream >> linearVelocity.x_ >> linearVelocity.y_ >> linearVelocity.z_;
  Vec3 angularVelocity;
  stream >> angularVelocity.x_ >> angularVelocity.y_ >> angularVelocity.z_;
  uint8_t bodyType;
  stream >> bodyType;
  float mass;
  stream >> mass;
  float linearDamping;
  stream >> linearDamping;
  float angularDamping;
  stream >> angularDamping;
  float gravityScale;
  stream >> gravityScale;
  bool useGravity;
  stream >> useGravity;
  stream >> m_Internal->m_Flags;

  SetBodyType((RBType)bodyType);
  SetMass(mass);
  SetLinearDamping(linearDamping);
  SetAngularDamping(angularDamping);
  SetGravityScale(gravityScale);
  SetUseGravity(useGravity);
  SetFreezeRotationX(GetFreezeRotationX());
  SetFreezeRotationY(GetFreezeRotationY());
  SetFreezeRotationZ(GetFreezeRotationZ());
  SetIgnorePhysics(GetIgnorePhysics());
}

void CRigidBody::RegisterAsGhostProperty(GhostObject * ghostObject, NetAuthority netAuthority)
{
  GhostPropertyComponent<CRigidBody>* prop = new GhostPropertyComponent<CRigidBody>{ this, netAuthority };
  ghostObject->RegisterPropertyCustom(prop);
}

void CRigidBody::AddForce(const Vec3& force)
{
  assert(m_Internal);

  m_Internal->AddForce(force);
}

void CRigidBody::SynchroniseRigidBody()
{
  assert(m_Internal);

  m_Internal->SynchroniseRigidBody();
}

RigidBody* CRigidBody::GetInternal() const
{
  return m_Internal;
}

const RBType& CRigidBody::GetBodyType() const
{
  assert(m_Internal);

  return m_Internal->GetBodyType();
}

const Vec3& CRigidBody::GetForce() const
{
  assert(m_Internal);

  return m_Internal->GetForce();
}

const Vec3& CRigidBody::GetTorque() const
{
  assert(m_Internal);

  return m_Internal->GetTorque();
}

const Vec3& CRigidBody::GetLinearVelocity() const
{
  assert(m_Internal);

  return m_Internal->GetLinearVelocity();
}

const Vec3& CRigidBody::GetAngularVelocity() const
{
  assert(m_Internal);

  return m_Internal->GetAngularVelocity();
}

float CRigidBody::GetMass() const
{
  assert(m_Internal);
  
	return m_Internal->GetMass();
}

float CRigidBody::GetLinearDamping() const
{
  assert(m_Internal);

	return m_Internal->GetMass();
}

float CRigidBody::GetAngularDamping() const
{
  assert(m_Internal);

	return m_Internal->GetAngularDamping();
}

float CRigidBody::GetGravityScale() const
{
  assert(m_Internal);
  
	return m_Internal->GetGravityScale();
}

bool CRigidBody::GetUseGravity() const
{
  assert(m_Internal);

	return m_Internal->GetUseGravity();
}

bool CRigidBody::GetFreezeRotationX() const
{
  assert(m_Internal);

	return m_Internal->GetFreezeRotationX();
}

bool CRigidBody::GetFreezeRotationY() const
{
  assert(m_Internal);

	return m_Internal->GetFreezeRotationY();
}

bool CRigidBody::GetFreezeRotationZ() const
{
  assert(m_Internal);

	return m_Internal->GetFreezeRotationZ();
}

bool CRigidBody::GetIgnorePhysics() const
{
  assert(m_Internal);

	return m_Internal->GetIgnorePhysics();
}

void CRigidBody::SetBodyType(RBType type)
{
  assert(m_Internal);

  m_Internal->SetBodyType(type);
}

void CRigidBody::SetForce(const Vec3& force)
{
  assert(m_Internal);

  m_Internal->SetForce(force);
}

void CRigidBody::SetTorque(const Vec3& torque)
{
  assert(m_Internal);
  m_Internal->SetTorque(torque);
}

void CRigidBody::SetLinearVelocity(const Vec3& linearVelocity)
{
  assert(m_Internal);

  m_Internal->SetLinearVelocity(linearVelocity);
}

void CRigidBody::SetAngularVelocity(const Vec3& angularVelocity)
{
  assert(m_Internal);

  m_Internal->SetAngularVelocity(angularVelocity);
}

void CRigidBody::SetMass(float m)
{
  assert(m_Internal);

  m_Internal->SetMass(m);
}

void CRigidBody::SetLinearDamping(float value)
{
  assert(m_Internal);

  m_Internal->SetLinearDamping(value);
}

void CRigidBody::SetAngularDamping(float value)
{
  assert(m_Internal);

  m_Internal->SetAngularDamping(value);
}

void CRigidBody::SetGravityScale(float scale)
{
  assert(m_Internal);

  m_Internal->SetGravityScale(scale);
}

void CRigidBody::SetUseGravity(bool useGravity)
{
  assert(m_Internal);

  m_Internal->SetUseGravity(useGravity);
}

void CRigidBody::SetFreezeRotationX(bool freeze)
{
  assert(m_Internal);

  m_Internal->SetFreezeRotationX(freeze);
}

void CRigidBody::SetFreezeRotationY(bool freeze)
{
  assert(m_Internal);

  m_Internal->SetFreezeRotationY(freeze);
}

void CRigidBody::SetFreezeRotationZ(bool freeze)
{
  assert(m_Internal);

  m_Internal->SetFreezeRotationZ(freeze);
}

void CRigidBody::SetIgnorePhysics(bool flag)
{
  assert(m_Internal);

  m_Internal->SetIgnorePhysics(flag);
}
