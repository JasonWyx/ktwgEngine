#include "fcollider.h"
#include "aabb3.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "physics.h"
#include "plane.h"

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

  BroadPhase& broadphase = Physics::GetInstance().m_ContactManager.broadPhase_;
  broadphase.MoveProxy(m_BroadphaseId, aabb, displacement);
}

void FCollider::SetAsBox()
{
  for (int i = 0; i < 8; ++i)
    m_Vertices.emplace_back(Vec3());

  for (int i = 0; i < 6; ++i)
    m_Planes.emplace_back(Plane());
}

void FCollider::UpdateBox(const Vec3& extents)
{
  UpdateBox(m_Local, extents);
}

void FCollider::UpdateBox(const Transform& local, const Vec3& extents)
{
  // Create box vertices with given extents
  Vec3 vertices[8] = { Vec3{ -extents.x_, -extents.y_, -extents.z_ },
                       Vec3{ -extents.x_, -extents.y_,  extents.z_ },
                       Vec3{ -extents.x_,  extents.y_, -extents.z_ },
                       Vec3{ -extents.x_,  extents.y_,  extents.z_ },
                       Vec3{  extents.x_, -extents.y_, -extents.z_ },
                       Vec3{  extents.x_, -extents.y_,  extents.z_ },
                       Vec3{  extents.x_,  extents.y_, -extents.z_ },
                       Vec3{  extents.x_,  extents.y_,  extents.z_ }, };

  // Update the vertices of the box
  for (unsigned i = 0u; i < m_Vertices.size(); ++i)
    m_Vertices[i] = Multiply(local, vertices[i]);

  // Update the plane of the box
  m_Planes[0] = Plane(X_VECTOR.Negate(), vertices[0]);
  m_Planes[1] = Plane(X_VECTOR, vertices[7]);
  m_Planes[2] = Plane(Y_VECTOR.Negate(), vertices[0]);
  m_Planes[3] = Plane(Y_VECTOR, vertices[7]);
  m_Planes[4] = Plane(Z_VECTOR.Negate(), vertices[0]);
  m_Planes[5] = Plane(Z_VECTOR, vertices[7]);

  //SynchroniseProxy();
}

void FCollider::AddContactEdge(ContactEdge* contactEdge)
{
  m_ContactEdgeList.PushFront(contactEdge);
}

void FCollider::DestroyContacts()
{
  Physics& world = Physics::GetInstance();
  ContactEdge* contact_edge = m_ContactEdgeList.head_;
  while (contact_edge)
  {
    auto to_destroy = contact_edge;
    contact_edge = contact_edge->next_;
    world.m_ContactManager.RemoveContact(to_destroy->contact_);
  }
}
