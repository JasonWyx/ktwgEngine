#include "rigidbody.h"
#include "aabb3.h"

RigidBody::RigidBody(Entity& entity)
  : m_Owner{ &entity }, m_Type{ RBT_DYNAMIC }, m_Mass{ 1.0f }, m_InvMass{ 1.0f },
    m_LinearDamping{}, m_AngularDamping{ 0.5f }, m_GravityScale{ 1.0f },
    m_UseGravity{ true }, m_IslandId{ INVALID_ISLAND_ID }
{
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
  //if (transform_ == GetOwner()().GetTransform())
  //  return;

  // Synchronise with the owner transform
  //transform_ = GetOwner()().GetTransform();

  // Update our sweep variable
  //sweep_.worldCenterOld_ = sweep_.worldCenter_;
  //sweep_.orientationOld_ = sweep_.orientation_;
  //sweep_.worldCenter_ = transform_.GetPosition();
  //sweep_.orientation_ = transform_.GetRotation();

  SetAwake(true);

  if (m_Type == RBT_STATIC)
    SynchroniseProxies();
}

void RigidBody::SynchroniseTransform()
{
  // auto& transform = GetOwner()().GetTransform();

  // auto update = sweep_.GetTransform(1.0f);

  // Update Entity's transform
  // transform.SetPosition(update.GetPosition());
  // transform.SetRotation(update.GetRotation());

  // Update Body's transform
  // transform_.SetPosition(update.GetPosition());
  // transform_.SetRotation(update.GetRotation());
}

void RigidBody::SynchroniseProxies()
{
  //auto old_transform = sweep_.GetTransform(0.0f);
  //auto new_transform = transform_;
  //old_transform.SetScale(new_transform.GetScale());
  //
  //auto displacement = new_transform.GetPosition() - old_transform.GetPosition();
  //
  //auto broadphase = &world_->contactManager_.broadPhase_;

  for (BoxCollider*& elem : m_Colliders)
  {
    AABB3 aabb1, aabb2;
    // elem->ComputeAABB(aabb1, old_transform);
    // elem->ComputeAABB(aabb2, new_transform);

    auto aabb = Combine(aabb1, aabb2);

    //broadphase->MoveProxy(dynamic_cast<BPFCollider*>(elem->GetInternal())->broadphaseID_, aabb, displacement);
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

  // Check all parents of this body, if parent is body, and this object is static, ignore
  // HEntity parent = static_cast<const Entity&>(this->GetOwner()()).GetParent();
  // while (parent.exist())
  // {
  //   auto bodyComp = parent().GetComponent<CRigidBody>();
  // 
  //   // Must have a Rigidbody component
  //   if (bodyComp)
  //   {
  //     if (static_cast<BPRigidBody*>(bodyComp->GetInternal()) == b && this->GetBodyType() == eBTStatic)
  //       return false;
  //   }
  // 
  //   parent = static_cast<const Entity&>(parent()).GetParent();
  // }

  // Check all parents of given body
  // parent = static_cast<const Entity&>(b->GetOwner()()).GetParent();
  // while (parent.exist())
  // {
  //   auto bodyComp = parent().GetComponent<CRigidBody>();
  // 
  //   // Must have a Rigidbody component
  //   if (bodyComp)
  //   {
  //     if (static_cast<BPRigidBody*>(bodyComp->GetInternal()) == this && b->GetBodyType() == eBTStatic)
  //       return false;
  //   }
  // 
  //   parent = static_cast<const Entity&>(parent()).GetParent();
  // }
  // 
  // // Collision layers are set to not collide
  // if (!(GetBPPhysics().layerCollisionMatrix_[this->GetLayerId()] & (1 << (MAX_COLLISION_LAYER - 1 - body->GetLayerId()))))
  //   return false;

  return true;
}

void RigidBody::SynchroniseProxies()
{
  //auto old_transform = sweep_.GetTransform(0.0f);
  //auto new_transform = transform_;
  //old_transform.SetScale(new_transform.GetScale());

  //auto displacement = new_transform.GetPosition() - old_transform.GetPosition();

  //auto broadphase = &world_->contactManager_.broadPhase_;

  for (BoxCollider*& elem : m_Colliders)
  {
    AABB3 aabb1, aabb2;
    //elem->ComputeAABB(aabb1, old_transform);
    //elem->ComputeAABB(aabb2, new_transform);

    auto aabb = Combine(aabb1, aabb2);

    //broadphase->MoveProxy(dynamic_cast<BPFCollider*>(elem->GetInternal())->broadphaseID_, aabb, displacement);
  }
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

  //for (auto& collider : m_Colliders)
  //  collider->SetActive(active);
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
    //force_.SetZero();
    //torque_.SetZero();
    //linearVelocity_.SetZero();
    //angularVelocity_.SetZero();
  }
}

void RigidBody::SetIgnorePhysics(bool flag)
{
  if (flag)
    m_Flags |= RBF_IGNOREPHYSICS;
  else
    m_Flags &= ~RBF_IGNOREPHYSICS;
}

void RigidBody::DestroyContacts()
{
  //for (auto& colliders : m_Colliders)
  //  colliders->DestroyContacts();
}

void RigidBody::ComputeInertia()
{
  // No need to compute inertia when there's no collider
  //if (m_Colliders_.empty())
  //  return;

  //inertia_.SetZero();
  //invInertia_.SetZero();
  //worldInvInertia_.SetZero();
  //sweep_.localCenter_.SetZero();

  // Static body has no mass and inertia
  if (m_Type == RBT_STATIC)
  {
    //sweep_.worldCenterOld_ = sweep_.worldCenter_ = transform_.GetPosition();
    //sweep_.orientationOld_ = sweep_.orientation_;
    m_InvMass = m_Mass = 0.0f;
    return;
  }

  if (m_Mass <= FLT_EPSILON)
    m_Mass = 1.0f;

  // Compute center of mass
  //Vec3 center{};

  /*for (auto& elem : m_Colliders)
  {
    MassData massData;
    elem->ComputeMassData(massData);

    center += massData.mass_ * massData.center_;
    inertia_ += massData.inertia_;
  }*/


  // Compute local center of mass
  m_InvMass = 1.0f / m_Mass;
  //center *= invMass_;

  //Matrix3 identity{};
  //identity.SetIdentity();

  //inertia_ -= (identity * Dot(center, center) - OuterProductMat3(center, center)) * mass_;
  //invInertia_ = Inverse(inertia_);
  //worldInvInertia_ = RotateToFrame(invInertia_, transform_.GetRotation().ToMat33());

  // Fix rotation.
  //if (m_Flags & RBF_FIXEDROTATIONX)
  //{
    //invInertia_.m_[1][1] = 0.0f;
    //invInertia_.m_[2][1] = 0.0f;
    //invInertia_.m_[1][2] = 0.0f;
    //invInertia_.m_[2][2] = 0.0f;

    //worldInvInertia_.m_[1][1] = 0.0f;
    //worldInvInertia_.m_[2][1] = 0.0f;
    //worldInvInertia_.m_[1][2] = 0.0f;
    //worldInvInertia_.m_[2][2] = 0.0f;
  //}

  //if (m_Flags & RBF_FIXEDROTATIONY)
  //{
    //invInertia_.m_[0][0] = 0.0f;
    //invInertia_.m_[0][2] = 0.0f;
    //invInertia_.m_[2][0] = 0.0f;
    //invInertia_.m_[2][2] = 0.0f;

    //worldInvInertia_.m_[0][0] = 0.0f;
    //worldInvInertia_.m_[0][2] = 0.0f;
    //worldInvInertia_.m_[2][0] = 0.0f;
    //worldInvInertia_.m_[2][2] = 0.0f;
  //}

  //if (m_Flags & RBF_FIXEDROTATIONZ)
  //{
    //invInertia_.m_[0][0] = 0.0f;
    //invInertia_.m_[0][1] = 0.0f;
    //invInertia_.m_[1][0] = 0.0f;
    //invInertia_.m_[1][1] = 0.0f;

    //worldInvInertia_.m_[0][0] = 0.0f;
    //worldInvInertia_.m_[0][1] = 0.0f;
    //worldInvInertia_.m_[1][0] = 0.0f;
    //worldInvInertia_.m_[1][1] = 0.0f;
  //}

  // Move center of mass.
  //sweep_.localCenter_ = Vec3{ 0,0,0 };
  //sweep_.worldCenterOld_ = sweep_.worldCenter_;
  //sweep_.worldCenter_ = Multiply(transform_, sweep_.localCenter_);

  // Update center of mass velocity.
  //linearVelocity_ += Cross(angularVelocity_, sweep_.worldCenter_ - sweep_.worldCenterOld_);
}
