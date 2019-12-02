#include "fcollider.h"
#include "aabb3.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "physics.h"
#include "plane.h"

FCollider::FCollider()
  : m_Vertices{}, m_Planes{}, 
    m_Friction{ 1.0f }, m_Restitution{ 0.0f },
    m_Local{}, m_Body{ nullptr }, m_Owner{ nullptr }, m_BroadphaseId{ -1 },
    m_IsTrigger{ false }, m_Active{ true }
{
  m_Local.SetIdentity();
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
  
  if (m_Body)
    UpdateBox(m_Local, UNIT_EXTENTS);
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

  SynchroniseProxy();
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

void FCollider::SetActive(bool active)
{
  if (m_Active == active)
    return;

  m_Active = active;

  if (!active)
  {
    if (m_BroadphaseId > -1)
    {
      m_Owner->DestroyContacts();
      Physics::GetInstance().m_ContactManager.broadPhase_.DestroyProxy(GetBroadphaseId());
      m_BroadphaseId = -1;
    }
  }
  else
  {
    if (m_BroadphaseId == -1)
    {
      AABB3 aabb;
      m_Owner->ComputeAABB(aabb, GetBody()->GetTransform());
      Physics::GetInstance().m_IsNewCollider = true;
      m_BroadphaseId = Physics::GetInstance().m_ContactManager.broadPhase_.CreateProxy(aabb, m_Owner);
    }
  }
}

void FCollider::Set(FCollider* fCollider)
{
  SetFriction(fCollider->m_Friction);
  SetRestitution(fCollider->m_Restitution);
  SetIsTrigger(fCollider->m_IsTrigger);
  SetActive(fCollider->m_Active);

  m_Local = fCollider->m_Local;

  for (size_t i = 0; i < m_Vertices.size(); ++i)
    m_Vertices[i] = fCollider->m_Vertices[i];

  for (size_t i = 0; i < m_Planes.size(); ++i)
    m_Planes[i] = fCollider->m_Planes[i];
}
