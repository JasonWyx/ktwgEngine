#include "boxcollider.h"
#include "fcollider.h"
#include "rigidbody.h"
#include "plane.h"

BoxCollider::BoxCollider(uint32_t id)
  : m_Internal{ nullptr }, 
    m_Extents { UNIT_EXTENTS }, 
    m_Min{}, m_Max{},
    m_Id{ id }
{
  m_Internal = std::make_unique<FCollider>();
  m_Internal->SetAsBox();
  m_Internal->SetOwner(this);
}

BoxCollider::~BoxCollider()
{
}

bool BoxCollider::Contains(const Vec3& point)
{
  AABB3 aabb;

  ComputeAABB(aabb, GetRigidBody()->GetTransform());

  return aabb.Contains(point);
}

void BoxCollider::ComputeMassData(MassData& massData)
{
  auto extent = GetExtents();

  // mass = volume * density
  // volume = width * height * depth
  // width = extents_.x * 2, height = extents_.y * 2, depth = extents_.z * 2
  float width  = extent.x_ * 2.0f;
  float height = extent.y_ * 2.0f;
  float depth  = extent.z_ * 2.0f;

  // Calculate inertia tensor
  float xx = width * width;
  float yy = height * height;
  float zz = depth * depth;

  auto bodyMass = m_Internal->GetBody()->GetMass() / m_Internal->GetBody()->GetColliders().size();

  // diagonal inertia in the inertia tensor matrix of a cuboid
  // Ixx = 1/12 * mass * (height^2 + depth^2)
  // Iyy = 1/12 * mass * (width^2 + depth^2)
  // Izz = 1/12 * mass * (width^2 + height^2)
  float Ixx = 1.0f / 12.0f * bodyMass * (yy + zz);
  float Iyy = 1.0f / 12.0f * bodyMass * (xx + zz);
  float Izz = 1.0f / 12.0f * bodyMass * (xx + yy);

  // Build inertia in local space
  Matrix3 inertia{};
  inertia.SetIdentity();
  inertia.m_[0][0] = Ixx;
  inertia.m_[1][1] = Iyy;
  inertia.m_[2][2] = Izz;

  Matrix3 identity{};
  identity.SetIdentity();
  inertia += (identity * Dot(m_Internal->GetCenter(), m_Internal->GetCenter()) - OuterProductMat3(m_Internal->GetCenter(), m_Internal->GetCenter())) * bodyMass;

  massData.m_Center = m_Internal->GetCenter();
  massData.m_Inertia = inertia;
  massData.m_Mass = bodyMass;
}

void BoxCollider::ComputeAABB(AABB3& aabb, const Transform& transform)
{
  Vec3 v[8];

  auto extent = GetExtents();

  const Vec3 boxVertices[8]{ Vec3(-extent.x_, -extent.y_, -extent.z_),
                             Vec3(-extent.x_, -extent.y_,  extent.z_),
                             Vec3(-extent.x_,  extent.y_, -extent.z_),
                             Vec3(-extent.x_,  extent.y_,  extent.z_),
                             Vec3(extent.x_, -extent.y_, -extent.z_),
                             Vec3(extent.x_, -extent.y_,  extent.z_),
                             Vec3(extent.x_,  extent.y_, -extent.z_),
                             Vec3(extent.x_,  extent.y_,  extent.z_) };

  // Find min and max of all vertex
  Vec3 min{ FLT_MAX,  FLT_MAX,  FLT_MAX };
  Vec3 max{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

  // Transform to world coordinate system
  for (auto i = 0u; i < 8; ++i)
  {
    v[i] = Multiply(transform, Multiply(GetLocal(), boxVertices[i]));
    min = MinVec(min, v[i]);
    max = MaxVec(max, v[i]);
  }

  // Set the aabb
  m_Min = aabb.m_Min = min;
  m_Max = aabb.m_Max = max;
}

bool BoxCollider::RayCast(const RayCastInput & input, RayCastOutput & output)
{
  // Need rotation matrix to get ray vector to collider local space
  Transform transform = GetRigidBody()->GetTransform();
  const Vec3& worldScale = transform.GetScale();

  Transform xf;
  xf.SetPosition(worldScale * GetLocal().GetPosition());

  // World transform of collider
  transform = Multiply(transform, xf);

  Matrix3 rotMat = transform.GetRotationMatrix33();

  // Put ray into frame of reference
  Vec3 p0 = MultiplyT(transform, input.m_p);
  Vec3 p1 = MultiplyT(transform, input.m_q);

  Vec3 d = p1 - p0;

  float tmin = 0.0f;
  float tmax = input.m_MaxT;

  FCollider* fCollider = m_Internal.get();

  int plane_count = 6; // Box always has 6 planes
  const std::vector<Plane>& planes = fCollider->GetPlanes();

  // Use to keep track of the plane intersected
  int index = INT_MAX;

  // Need to do plane and ray intersection test 
  for (int i = 0; i < plane_count; ++i)
  {
     float numerator = planes[i].d_ - Dot(planes[i].normal_, p0);
     float denominator = Dot(planes[i].normal_, d);

    if (denominator == 0.0f)
    {
      // ray is parallel to halfspace
      if (numerator < 0.0f)
        return false; // Ray is outside of this halfspace
    }
    else
    {
      if (denominator < 0.0f)
      {
        if (numerator < tmin * denominator)
        {
          tmin = numerator / denominator;
          index = i;
        }
      }
      else
      {
        if (numerator < tmax * denominator)
          tmax = numerator / denominator;
      }
    
      if (tmax < tmin)
        return false;
    }
  }

  if (index != INT_MAX)
  {
    output.m_T = tmin;
    output.m_Normal = Multiply(rotMat, planes[index].normal_);
    output.m_Collider = this;
    output.m_Owner = m_Internal->GetBody()->GetOwner();
    return true;
  }

  return false;
}

uint32_t BoxCollider::GetSupportFace(const Vec3 & direction)
{
  const std::vector<Plane>& plane = m_Internal->GetPlanes();
  int planeCount = 6; // Box has 6 face

  int index = 0;
  float maxProjection = Dot(direction, plane[0].normal_);
  for (int i = 1; i < planeCount; ++i)
  {
    float projection = Dot(direction, plane[i].normal_);
    if (projection > maxProjection)
    {
      index = i;
      maxProjection = projection;
    }
  }

  return index;
}

void BoxCollider::AddContactEdge(ContactEdge* contactEdge)
{
  m_Internal->AddContactEdge(contactEdge);
}

void BoxCollider::DestroyContacts()
{
  m_Internal->DestroyContacts();
}

RigidBody* BoxCollider::GetRigidBody() const
{
  return m_Internal->GetBody();
}

const Transform& BoxCollider::GetLocal() const
{
  return m_Internal->GetLocal();
}

FCollider* BoxCollider::GetInternal() const
{
  return m_Internal.get();
}

const std::vector<Vec3>& BoxCollider::GetVertices() const
{
  return m_Internal->GetVertices();
}

const Vec3& BoxCollider::GetCenter() const
{
  return m_Internal->GetCenter();
}

float BoxCollider::GetRadius() const
{
  return 0.0f;
}

int32_t BoxCollider::GetBroadphaseId() const
{
  return m_Internal->GetBroadphaseId();
}

bool BoxCollider::GetIsTrigger() const
{
  return m_Internal->GetIsTrigger();
}

bool BoxCollider::GetActive() const
{
  return m_Internal->GetActive();
}

float BoxCollider::GetFriction() const
{
  return m_Internal->GetFriction();
}

float BoxCollider::GetRestitution() const
{
  return m_Internal->GetRestitution();
}

const Vec3& BoxCollider::GetExtents() const
{
  return m_Extents;
}

const Vec3 & BoxCollider::GetMin() const
{
  return m_Min;
}

const Vec3 & BoxCollider::GetMax() const
{
  return m_Max;
}

void BoxCollider::SetRigidBody(RigidBody* rigidBody)
{
  m_Internal->SetBody(rigidBody);
}

void BoxCollider::SetBroadphaseId(int32_t id)
{
  m_Internal->SetBroadphaseId(id);
}

void BoxCollider::SetExtents(const Vec3& extent)
{
  m_Extents = extent;
}

void BoxCollider::SetCenter(const Vec3& center)
{
  m_Internal->SetCenter(center);
}

void BoxCollider::SetIsTrigger(bool isTrigger)
{
  m_Internal->SetIsTrigger(isTrigger);
}

void BoxCollider::SetActive(bool active)
{
  m_Internal->SetActive(active);
}
