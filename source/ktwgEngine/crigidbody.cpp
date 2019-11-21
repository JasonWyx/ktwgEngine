#include "crigidbody.h"

CRigidBody::CRigidBody(Entity& entity, uint32_t id)
  : Component{ entity, id }, m_RBType{ RBT_DYNAMIC }, m_Mass{ 1.0f },
  m_InvMass{ 1.0f }, m_LinearDamping{}, m_AngularDamping{ 0.5f }, m_GravityScale{ 1.0f },
  m_UseGravity{ true }, m_IslandId{}, m_Flags{ 0 }, m_SleepTime{ 0.0f },
  m_World{ nullptr }
{
  // Synchronise our initial position with the owner's position
  // auto& ent = const_cast<Entity&>(*entity);
  //transform_ = ent().GetTransform();

  // First creation of a rigidbody is always awake
  m_Flags |= RBF_AWAKE;

  // Make sure our variables are initialised
  //inertia_.SetZero();
  //invInertia_.SetZero();
  //worldInvInertia_.SetZero();

  // Initialise our sweep for TOI collision
  //sweep_.localCenter_.SetZero();
  //sweep_.worldCenter_ = sweep_.worldCenterOld_ = transform_.GetPosition();
  //sweep_.orientation_ = sweep_.orientationOld_ = transform_.GetRotation();
  //sweep_.t_ = 0.0f;

  SetActive(true);
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Initialize()
{
}

void CRigidBody::Destroy()
{
}

void CRigidBody::SetActive(bool active)
{
}

void CRigidBody::SynchroniseRigidBody()
{
}

const RBType& CRigidBody::GetBodyType() const
{
  return m_RBType;
}

float CRigidBody::GetMass() const
{
	return 0.0f;
}

float CRigidBody::GetLinearDamping() const
{
	return 0.0f;
}

float CRigidBody::GetAngularDamping() const
{
	return 0.0f;
}

float CRigidBody::GetGravityScale() const
{
	return 0.0f;
}

bool CRigidBody::GetUseGravity() const
{
	return false;
}

bool CRigidBody::GetFreezeRotationX() const
{
	return false;
}

bool CRigidBody::GetFreezeRotationY() const
{
	return false;
}

bool CRigidBody::GetFreezeRotationZ() const
{
	return false;
}

bool CRigidBody::GetIgnorePhysics() const
{
	return false;
}

void CRigidBody::SetBodyType(RBType type)
{
}

void CRigidBody::SetMass(float m)
{
}

void CRigidBody::SetLinearDamping(float value)
{
}

void CRigidBody::SetAngularDamping(float value)
{
}

void CRigidBody::SetGravityScale(float scale)
{
}

void CRigidBody::SetUseGravity(bool useGravity)
{
}

void CRigidBody::SetFreezeRotationX(bool freeze)
{
}

void CRigidBody::SetFreezeRotationY(bool freeze)
{
}

void CRigidBody::SetFreezeRotationZ(bool freeze)
{
}

void CRigidBody::SetIgnorePhysics(bool flag)
{
}

