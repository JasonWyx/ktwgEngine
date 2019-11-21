#include "crigidbody.h"
#include "physics.h"

CRigidBody::CRigidBody(Entity& entity, uint32_t id)
  : Component{ entity, id }, m_Internal{ nullptr }
{
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
}

void CRigidBody::SetActive(bool active)
{
  Component::SetActive(active);

  assert(m_Internal);

  m_Internal->SetActive(active);
}

void CRigidBody::SynchroniseRigidBody()
{
  assert(m_Internal);

  m_Internal->SynchroniseRigidBody();
}

const RBType& CRigidBody::GetBodyType() const
{
  assert(m_Internal);

  return m_Internal->GetBodyType();
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
