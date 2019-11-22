#pragma once

#include <memory>
#include <vector>
#include "physicsconstants.h"
#include "vector3.h"
#include "matrix3.h"
#include "aabb3.h"
#include "raycast.h"
#include "transform.h"

class Entity;
class FCollider;
class RigidBody;

struct MassData
{
  Matrix3 m_Inertia = Matrix3{};
  Vec3    m_Center  = Vec3{};
  float   m_Mass    = 1.0f;
};

class BoxCollider
{
  template <typename T>
  using UniquePtr = std::unique_ptr<T>;
  
public:
  BoxCollider();
  ~BoxCollider();

  /* Query if point is contained in collider */
  bool Contains(const Vec3& point);

  /* Compute mass data */
  void ComputeMassData(MassData& massData);

  /* Compute AABB data */
  //void ComputeAABB(AABB3& aabb, const Transform& transform);

  /* Test Ray cast against box */
  bool RayCast(const RayCastInput& input, RayCastOutput& output);

  /* Test sphere cast on collider shape*/
  //bool SphereCast(const RayCastInput& input, float radius, RayCastOutput& output);

  /* Get the support face given a direction ( support face is face furthest in direction ) */
  uint32_t GetSupportFace(const Vec3& direction);

  /* Interface functions : Getters */
  RigidBody*               GetRigidBody() const;
  const Transform&         GetLocal()     const;
  const std::vector<Vec3>& GetVertices()  const;
  const Vec3&              GetCenter()    const;
  bool                     GetIsTrigger() const;
  bool                     GetActive()    const;
  float                    GetRadius()    const;
  const Vec3&              GetExtents()   const;
  const Vec3&              GetMin()       const;
  const Vec3&              GetMax()       const;

  /* Interface functions : Setters */
  void SetExtents(const Vec3& extent);
  void SetCenter(const Vec3& center);
  void SetIsTrigger(bool isTrigger);
  void SetActive(bool active);

private:
  UniquePtr<FCollider> m_Internal;

  Vec3 m_Extents; // unit extents for each axis of the box collider
  Vec3 m_Min;
  Vec3 m_Max;
};