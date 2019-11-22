#include "fcollider.h"
#include "aabb3.h"
#include "boxcollider.h"
#include "rigidbody.h"

FCollider::FCollider()
  : m_Vertices{}, m_Planes{}, m_Friction{ 0.0f }, m_Restitution{ 0.0f },
    m_Body{ nullptr }, m_Owner{ nullptr }, m_BroadphaseId{ -1 },
    m_IsTrigger{ false }, m_Active{ true }
{
}

FCollider::~FCollider()
{
  CleanUp();
}

void FCollider::CleanUp()
{
  container_t<Vec3> emptyVertices;
  container_t<Plane> emptyPlanes;

  std::swap(emptyVertices, m_Vertices);
  std::swap(emptyPlanes, m_Planes);
}

void FCollider::SynchroniseProxy()
{
  AABB3 aabb;
  Vec3 displacement;
  displacement.SetZero();

  m_Owner->ComputeAABB(aabb, m_Body->GetTransform());

  //auto broadphase = &dynamic_cast<BPRigidBody*>(owner_->GetRigidBody())->world_->contactManager_.broadPhase_;
  //broadphase->MoveProxy(broadphaseID_, aabb, displacement);
}

void FCollider::SetAsBox()
{
  for (int i = 0; i < 8; ++i)
    m_Vertices.emplace_back(std::make_unique<Vec3>());

  for (int i = 0; i < 6; ++i)
    m_Planes.emplace_back(std::make_unique<Plane>());
}

void FCollider::UpdateBox(const Vec3 & extents)
{
}

void FCollider::UpdateBox(const Transform & local, const Vec3 & extents)
{
}
