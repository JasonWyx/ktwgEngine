#pragma once

#include <memory>
#include <vector>
#include "vector3.h"
#include "transform.h"
#include "customlist.h"
#include "contact.h"

class RigidBody;
class BoxCollider;
struct Plane;

class FCollider
{
  template <typename T>
  using container_t = std::vector<T>;

  friend class Physics;
  friend class ContactManager;

public:
  FCollider();
  ~FCollider();

  void CleanUp();
  void SynchroniseProxy();

  void SetAsBox();
  void UpdateBox(const Vec3& extents);
  void UpdateBox(const Transform& local, const Vec3& extents);

  void AddContactEdge(ContactEdge* contactEdge);
  void DestroyContacts();

  inline const container_t<Vec3>&  GetVertices()     const { return m_Vertices; }
  inline const container_t<Plane>& GetPlanes()       const { return m_Planes; }
  inline float                     GetFriction()     const { return m_Friction; }
  inline float                     GetRestitution()  const { return m_Restitution; }
  inline const Vec3&               GetCenter()       const { return m_Local.GetPosition(); }
  inline const Transform&          GetLocal()        const { return m_Local; }
  inline RigidBody*                GetBody()         const { return m_Body; }
  inline BoxCollider*              GetOwner()        const { return m_Owner; }
  inline int32_t                   GetBroadphaseId() const { return m_BroadphaseId; }
  inline bool                      GetIsTrigger()    const { return m_IsTrigger; }
  inline bool                      GetActive()       const { return m_Active; }

  inline void SetFriction(float friction)           { m_Friction = friction; }
  inline void SetRestitution(float restitution)     { m_Restitution = restitution; }
  inline void SetCenter(const Vec3& center)         { m_Local.SetPosition(center); }
  inline void SetBody(RigidBody* body)              { m_Body = body; }
  inline void SetOwner(BoxCollider* owner)          { m_Owner = owner; }
  inline void SetBroadphaseId(int32_t broadphaseId) { m_BroadphaseId = broadphaseId; }
  inline void SetIsTrigger(bool isTrigger)          { m_IsTrigger = isTrigger; }

  void SetActive(bool active);
  void Set(FCollider* fCollider);

private:
  container_t<Vec3> m_Vertices;
  container_t<Plane> m_Planes;

  // Material attribute
  float m_Friction;
  float m_Restitution;

  Transform    m_Local;
  RigidBody*   m_Body;
  BoxCollider* m_Owner;
  int32_t      m_BroadphaseId;
  bool         m_IsTrigger;
  bool         m_Active;
  
  List<ContactEdge> m_ContactEdgeList; // List of contact edge made with other colliders
};
