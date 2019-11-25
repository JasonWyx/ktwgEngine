#include "rigidbody.h"
#include "aabb3.h"
#include "boxcollider.h"
#include "fcollider.h"
#include "physics.h"
#include "entity.h"

RigidBody::RigidBody(Entity& entity)
  : m_Owner{ &entity }, 
    m_Type{ RBT_DYNAMIC }, 
    m_Mass{ 1.0f }, 
    m_InvMass{ 1.0f },
    m_LinearDamping{}, m_AngularDamping{ 0.5f }, 
    m_GravityScale{ 1.0f }, m_UseGravity{ true }, 
    m_IslandId{ INVALID_ISLAND_ID },
    m_Active{ true }
{
  // Synchronise our initial position with the owner's position
  m_Transform = entity.GetTransform();

  // First creation of a rigidbody is always awake
  m_Flags |= RBF_AWAKE;

  // Make sure our variables are initialised
  m_Inertia.SetZero();
  m_InvInertia.SetZero();
  m_WorldInvInertia.SetZero();

  // Initialise our sweep for TOI collision
  m_Sweep.localCenter_.SetZero();
  m_Sweep.worldCenter_ = m_Sweep.worldCenterOld_ = m_Transform.GetPosition();
  m_Sweep.orientation_ = m_Sweep.orientationOld_ = m_Transform.GetRotation();
  m_Sweep.t_ = 0.0f;
}

RigidBody::~RigidBody()
{
}

void RigidBody::AddForce(const Vec3 & force)
{
  // Cannot add force to a non dynamic body
  if (m_Type != RBT_DYNAMIC)
    return;

  // Wake the body up as force is applied if body is asleep
  if (!IsAwake())
    SetAwake(true);

  // Only able to apply force on body which is awake
  if (IsAwake())
    m_Force += force;
}

void RigidBody::SynchroniseRigidBody()
{
  // No need to synchronise inactive body
  if (!m_Active)
    return;

  // If our old transform and new transform is different, we need to update
  if (m_Transform == GetOwner()->GetTransform())
    return;

  // Synchronise with the owner transform
  m_Transform = GetOwner()->GetTransform();

  // Update our sweep variable
  m_Sweep.worldCenterOld_ = m_Sweep.worldCenter_;
  m_Sweep.orientationOld_ = m_Sweep.orientation_;
  m_Sweep.worldCenter_ = m_Transform.GetPosition();
  m_Sweep.orientation_ = m_Transform.GetRotation();

  SetAwake(true);

  if (m_Type == RBT_STATIC)
    SynchroniseProxies();
}

void RigidBody::SynchroniseTransform()
{
  Transform& transform = GetOwner()->GetTransform();

  auto update = m_Sweep.GetTransform(1.0f);

  // Update Entity's transform
  transform.SetPosition(update.GetPosition());
  transform.SetRotation(update.GetRotation());

  // Update Body's transform
  m_Transform.SetPosition(update.GetPosition());
  m_Transform.SetRotation(update.GetRotation());
}

void RigidBody::SynchroniseProxies()
{
  Transform old_transform = m_Sweep.GetTransform(0.0f);
  Transform new_transform = m_Transform;
  old_transform.SetScale(new_transform.GetScale());
  
  Vec3 displacement = new_transform.GetPosition() - old_transform.GetPosition();
  
  BroadPhase& broadphase = Physics::GetInstance().m_ContactManager.broadPhase_;

  for (UniquePtr<BoxCollider>& elem : m_Colliders)
  {
    AABB3 aabb1, aabb2;
    elem->ComputeAABB(aabb1, old_transform);
    elem->ComputeAABB(aabb2, new_transform);

    AABB3 aabb = Combine(aabb1, aabb2);

    broadphase.MoveProxy(elem->GetInternal()->GetBroadphaseId(), aabb, displacement);
  }
}

bool RigidBody::CanCollide(RigidBody* body)
{
  // At least one body must be dynamic for collision to happen
  if (this->GetBodyType() != RBT_DYNAMIC && body->GetBodyType() != RBT_DYNAMIC)
    return false;

  // Same body cannot collide
  if (this == body)
    return false;
  
   // Collision layers are set to not collide
   //if (!(Physics::GetInstance().m_LayerCollisionMatrix[this->GetLayerId()] & (1 << (MAX_COLLISION_LAYER - 1 - body->GetLayerId()))))
   //  return false;

  return true;
}

uint32_t RigidBody::GetLayerId() const
{
  return m_Owner->GetCollisionLayer();
}

void RigidBody::SetBodyType(const RBType& type)
{
  m_Type = type;
}

void RigidBody::SetForce(const Vec3& force)
{
  m_Force = force;
}

void RigidBody::SetTorque(const Vec3& torque)
{
  m_Torque = torque;
}

void RigidBody::SetLinearVelocity(const Vec3& linearVelocity)
{
  m_LinearVelocity = linearVelocity;
}

void RigidBody::SetAngularVelocity(const Vec3& angularVelocity)
{
  m_AngularVelocity = angularVelocity;
}

void RigidBody::SetMass(float m)
{
  m_Mass = m;
}

void RigidBody::SetLinearDamping(float linearDamping)
{
  m_LinearDamping = linearDamping;
}

void RigidBody::SetAngularDamping(float angularDamping)
{
  m_AngularDamping = angularDamping;
}

void RigidBody::SetGravityScale(float gravityScale)
{
  m_GravityScale = gravityScale;
}

void RigidBody::SetUseGravity(bool useGravity)
{
  m_UseGravity = useGravity;
}

void RigidBody::SetFreezeRotationX(bool freeze)
{
  if (freeze)
  {
    m_Flags |= RBF_FIXEDROTATIONX;

  }
  else
  {
    m_Flags &= RBF_FIXEDROTATIONX;
  }

  ComputeInertia();
}

void RigidBody::SetFreezeRotationY(bool freeze)
{
  if (freeze)
  {
    m_Flags |= RBF_FIXEDROTATIONY;
  }
  else
  {
    m_Flags &= ~RBF_FIXEDROTATIONY;
  }

  ComputeInertia();
}

void RigidBody::SetFreezeRotationZ(bool freeze)
{
  if (freeze)
  {
    m_Flags |= RBF_FIXEDROTATIONZ;
  }
  else
  {
    m_Flags &= ~RBF_FIXEDROTATIONZ;
  }

  ComputeInertia();
}

void RigidBody::SetActive(bool active)
{
  m_Active = active;

  for (auto& collider : m_Colliders)
    collider->SetActive(active);
}

void RigidBody::SetAwake(bool flag)
{
  if (flag)
  {
    if (!IsAwake())
    {
      m_Flags |= RBF_AWAKE;
      m_SleepTime = 0.0f;
    }
  }
  else
  {
    m_Flags &= RBF_AWAKE;
    m_SleepTime = 0.0f;
    m_Force.SetZero();
    m_Torque.SetZero();
    m_LinearVelocity.SetZero();
    m_AngularVelocity.SetZero();
  }
}

void RigidBody::SetIgnorePhysics(bool flag)
{
  if (flag)
    m_Flags |= RBF_IGNOREPHYSICS;
  else
    m_Flags &= ~RBF_IGNOREPHYSICS;
}

BoxCollider* RigidBody::CreateCollider(uint32_t id)
{
  BoxCollider* collider = nullptr;

  // Create the correct collider
  m_Colliders.emplace_back(std::make_unique<BoxCollider>(id));
  collider = m_Colliders.back().get();
  collider->SetRigidBody(this);

  ComputeInertia();

  // Place collider in the broadphase
  AABB3 aabb;
  collider->ComputeAABB(aabb, m_Transform);
  collider->SetBroadphaseId(Physics::GetInstance().m_ContactManager.broadPhase_.CreateProxy(aabb, collider));

  // A new collider is created
  Physics::GetInstance().m_IsNewCollider = true;

  // collider->SetActive(true);

  return collider;
}

void RigidBody::DestroyCollider(BoxCollider* collider)
{
  for (size_t i = 0U; i < m_Colliders.size(); ++i)
  {
    if (m_Colliders[i].get() == collider)
    {
      collider->DestroyContacts();

      if (collider->GetBroadphaseId() != -1)
      {
        Physics::GetInstance().m_ContactManager.broadPhase_.DestroyProxy(collider->GetBroadphaseId());
        collider->SetBroadphaseId(-1);
      }

      m_Colliders[i].release();
      m_Colliders.erase(m_Colliders.begin() + i);
      break;
    }
  }

  // Need to recompute inertia
  ComputeInertia();
}

void RigidBody::DestroyContacts()
{
  for (auto& colliders : m_Colliders)
    colliders->DestroyContacts();
}

void RigidBody::ComputeInertia()
{
  // No need to compute inertia when there's no collider
  if (m_Colliders.empty())
    return;

  m_Inertia.SetZero();
  m_InvInertia.SetZero();
  m_WorldInvInertia.SetZero();
  m_Sweep.localCenter_.SetZero();

  // Static body has no mass and inertia
  if (m_Type == RBT_STATIC)
  {
    m_Sweep.worldCenterOld_ = m_Sweep.worldCenter_ = m_Transform.GetPosition();
    m_Sweep.orientationOld_ = m_Sweep.orientation_;
    m_InvMass = m_Mass = 0.0f;
    return;
  }

  if (m_Mass <= FLT_EPSILON)
    m_Mass = 1.0f;

  // Compute center of mass
  Vec3 center{};

  for (auto& elem : m_Colliders)
  {
    MassData massData;
    elem->ComputeMassData(massData);

    center += massData.m_Mass * massData.m_Center;
    m_Inertia += massData.m_Inertia;
  }

  // Compute local center of mass
  m_InvMass = 1.0f / m_Mass;
  center *= m_InvMass;

  Matrix3 identity{};
  identity.SetIdentity();

  m_Inertia -= (identity * Dot(center, center) - OuterProductMat3(center, center)) * m_Mass;
  m_InvInertia = Inverse(m_Inertia);
  m_WorldInvInertia = RotateToFrame(m_InvInertia, m_Transform.GetRotation().ToMat33());

  // Fix rotation.
  if (m_Flags & RBF_FIXEDROTATIONX)
  {
    m_InvInertia.m_[1][1] = 0.0f;
    m_InvInertia.m_[2][1] = 0.0f;
    m_InvInertia.m_[1][2] = 0.0f;
    m_InvInertia.m_[2][2] = 0.0f;

    m_WorldInvInertia.m_[1][1] = 0.0f;
    m_WorldInvInertia.m_[2][1] = 0.0f;
    m_WorldInvInertia.m_[1][2] = 0.0f;
    m_WorldInvInertia.m_[2][2] = 0.0f;
  }

  if (m_Flags & RBF_FIXEDROTATIONY)
  {
    m_InvInertia.m_[0][0] = 0.0f;
    m_InvInertia.m_[0][2] = 0.0f;
    m_InvInertia.m_[2][0] = 0.0f;
    m_InvInertia.m_[2][2] = 0.0f;

    m_WorldInvInertia.m_[0][0] = 0.0f;
    m_WorldInvInertia.m_[0][2] = 0.0f;
    m_WorldInvInertia.m_[2][0] = 0.0f;
    m_WorldInvInertia.m_[2][2] = 0.0f;
  }

  if (m_Flags & RBF_FIXEDROTATIONZ)
  {
    m_InvInertia.m_[0][0] = 0.0f;
    m_InvInertia.m_[0][1] = 0.0f;
    m_InvInertia.m_[1][0] = 0.0f;
    m_InvInertia.m_[1][1] = 0.0f;

    m_WorldInvInertia.m_[0][0] = 0.0f;
    m_WorldInvInertia.m_[0][1] = 0.0f;
    m_WorldInvInertia.m_[1][0] = 0.0f;
    m_WorldInvInertia.m_[1][1] = 0.0f;
  }

  // Move center of mass.
  m_Sweep.localCenter_ = Vec3{ 0,0,0 };
  m_Sweep.worldCenterOld_ = m_Sweep.worldCenter_;
  m_Sweep.worldCenter_ = Multiply(m_Transform, m_Sweep.localCenter_);

  // Update center of mass velocity.
  m_LinearVelocity += Cross(m_AngularVelocity, m_Sweep.worldCenter_ - m_Sweep.worldCenterOld_);
}
