#pragma once

#include "vector3.h"

class BoxCollider
{
public:
  BoxCollider(size_t id);
  ~BoxCollider();

  /* Query if point is contained in collider */
  bool Contains(const Vec3& point);

  /* Compute mass data */
  void ComputeMassData(MassData& massData);

  /* Compute AABB data */
  void ComputeAABB(AABB3& aabb, const Transform& transform);

  /* Test Ray cast against box */
  bool RayCast(const RayCastInput& input, RayCastOutput& output);

  /* Test sphere cast on collider shape*/
  bool SphereCast(const RayCastInput& input, float radius, RayCastOutput& output) override;

  /* Get the support face given a direction ( support face is face furthest in direction ) */
  uint32_t GetSupportFace(const Vec3& direction);

  /* Interface functions : Getters */
  const Transform& GetLocal() const;
  const Vec3&      GetExtents() const override;
  const Vec3&      GetMin() const override;
  const Vec3&      GetMax() const override;

  /* Interface functions : Setters */
  void SetExtents(const Vec3& extent) override;

  void Set(ICollider* rhs)  override;

private:
  Vec3 extents_; // unit extents for each axis of the box collider

  Vec3 min_;
  Vec3 max_;
};