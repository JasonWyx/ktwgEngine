#include "rigidbody.h"

CRigidBody::CRigidBody(const SharedPtr<Entity>& entity, uint32_t id)
  : Component{ entity, id }, m_RBType{ RBT_DYNAMIC }, m_Mass{ 1.0f },
  m_InvMass{ 1.0f }, m_LinearDamping{}, m_AngularDamping{ 0.5f }, m_GravityScale{ 1.0f },
  m_UseGravity{ true }, m_IslandId{}, m_Flags{ 0 }, m_SleepTime{ 0.0f },
  m_World{ nullptr }
{
  // Synchronise our initial position with the owner's position
  auto& ent = const_cast<Entity&>(*entity);
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
