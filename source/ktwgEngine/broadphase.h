#pragma once

#include "aabb3.h"
#include "dynamicaabb3tree.h"

constexpr auto PAIR_CAPACITY = 100;
constexpr auto MOVE_BUFFER_CAPACITY = 100;

class ContactManager;
struct CastCallback;
struct OverlapShapeCallback;
struct SphereShape;

struct ProxyPair
{
  int32_t proxyA;
  int32_t proxyB;
};

/* This class represent the broadphase algorithm checks to be used on the       *
 * physics system. We use a dynamic AABB tree to check if boxes are potentially *
 * in collision before generating them as a pair for the physics solver.        */
class BroadPhase
{
public:
  BroadPhase(ContactManager* manager);
  ~BroadPhase();

  void GeneratePairs();

  /* Provide a fat AABB and userdata to be added into our AABB tree */
  int32_t CreateProxy(const AABB3& aabb, void* data);

  /* Deallocate the proxy from our AABB tree */
  void DestroyProxy(int32_t index);

  /* Update the proxy with given aabb and displacement */
  bool MoveProxy(int32_t index, const AABB3& aabb, const Vec3& displacement);

  /* Retrieve user data */
  void* GetUserData(int32_t index) const;

  /* Test overlap between two fat AABB */
  bool TestOverlap(int32_t A, int32_t B) const;

  /* Test ray cast with broadphase */
  void RayCast(CastCallback* callback, const RayCastInput& input, bool hitTriggers) const;

  /* Test sphere cast with broadphase */
  //void SphereCast(CastCallback* callback, const RayCastInput& input, float radius, bool hitTriggers) const;

  /* Test overlap sphere */
  //void OverlapSphere(OverlapShapeCallback* callback, const SphereShape& input, bool hitTriggers) const;

  /* Retrieve the root of our nodes */
  int32_t GetTreeRoot() const;

  /* Retrieve the tree nodes */
  TreeNode* GetTreeNodes() const;

  /* Reset the tree to initial state */
  void Reset();
private:
  friend class DynamicAABB3Tree;
  friend class RigidBody;

  /* Add a proxy to our list of moved proxies to be used internally */
  void BufferMove(int32_t id);

  bool QueryCallback(int32_t id);

  ContactManager* contactManager_;     // Pointer to our contactManager/the owner of this broadphase algorithm
  ProxyPair*      pairsBuffer_;        // A pair buffer pre-allocated to store pairs of colliders in contact
  uint32_t        pairCount_;          // Current size of the pair buffer
  uint32_t        pairCapacity_;       // Capacity of the pair buffer

  int32_t*        moveBuffer_;         // Keeps track of our move buffer 
  int32_t         moveBufferCount_;    // Current number of move's notified
  int32_t         moveBufferCapacity_; // Capacity of move buffer

  int32_t         queryId_;

  DynamicAABB3Tree tree_;             // Dynamic AABB tree use to query broadphase check

};

/// This is used to sort pairs.
inline bool ProxyPairCompare(const ProxyPair& lhs, const ProxyPair& rhs)
{
  if (lhs.proxyA < rhs.proxyA)
    return true;

  if (lhs.proxyA == rhs.proxyA)
    return lhs.proxyB < rhs.proxyB;

  return false;
}