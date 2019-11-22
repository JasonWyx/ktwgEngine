#pragma once

#include <memory>
#include <vector>
#include "vector3.h"

class RigidBody;
class BoxCollider;
struct Plane;

class FCollider
{
  template <typename T>
  using container_t = std::vector<T>;

public:
  FCollider();
  ~FCollider();

  void CleanUp();
  void SynchroniseProxy();

  void SetAsBox();
  void UpdateBox(const Vec3& extents);
  //void UpdateBox(const Transform& local, const Vec3& extents);

  inline const container_t<Vec3>&  GetVertices()     const { return m_Vertices; }
  inline const container_t<Plane>& GetPlanes()       const { return m_Planes; }
  inline float                     GetFriction()     const { return m_Friction; }
  inline float                     GetRestitution()  const { return m_Restitution; }
  inline const Vec3&               GetCenter()       const { return Vec3{}; }
  inline RigidBody*                GetBody()         const { return m_Body; }
  inline BoxCollider*              GetOwner()        const { return m_Owner; }
  inline int32_t                   GetBroadphaseId() const { return m_BroadphaseId; }
  inline bool                      GetIsTrigger()    const { return m_IsTrigger; }
  inline bool                      GetActive()       const { return m_Active; }

  inline void SetFriction(float friction)           { m_Friction = friction; }
  inline void SetRestitution(float restitution)     { m_Restitution = restitution; }
  inline void SetBody(RigidBody* body)              { m_Body = body; }
  inline void SetOwner(BoxCollider* owner)          { m_Owner = owner; }
  inline void SetBroadphaseId(int32_t broadphaseId) { m_BroadphaseId = broadphaseId; }
  inline void SetIsTrigger(bool isTrigger)          { m_IsTrigger = isTrigger; }
  inline void SetActive(bool active)                { m_Active = active; }

private:
  container_t<Vec3> m_Vertices;
  container_t<Plane> m_Planes;

  // Material attribute
  float m_Friction;
  float m_Restitution;

  RigidBody*   m_Body;
  BoxCollider* m_Owner;
  int32_t      m_BroadphaseId;
  bool         m_IsTrigger;
  bool         m_Active;
  
  // BPList<BPContactEdge> contactEdgeList_; // List of contact edge made with other colliders
};
