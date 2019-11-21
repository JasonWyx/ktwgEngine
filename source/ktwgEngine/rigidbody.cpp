#include "rigidbody.h"

RigidBody::RigidBody(const Entity& entity)
  : m_Entity{ &entity }, m_Type{ RBT_DYNAMIC }, m_Mass{ 1.0f }, m_InvMass{ 1.0f },
    m_LinearDamping{}, m_AngularDamping{ 0.5f }, m_GravityScale{ 1.0f },
    m_UseGravity{ true }, m_IslandId{ INVALID_ISLAND_ID }
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::SynchroniseRigidBody()
{
}

void RigidBody::SynchroniseTransform()
{
}

void RigidBody::SynchroniseProxies()
{
}

bool RigidBody::CanCollide(RigidBody* body)
{
  return false;
}

void RigidBody::SetBodyType(const RBType& type)
{
  m_Type = type;
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
