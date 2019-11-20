#pragma once

#include <vector>
#include <memory>
#include "singleton.h"
#include "physicsconstants.h"

class Entity;
class RigidBody;

class Physics final : public Singleton<Physics>
{
  template <typename T>
  using container_t = std::vector<T>;

  template <typename T>
  using UniquePtr = std::unique_ptr<T>;

public:
  Physics();
  ~Physics();

  void InitializeInternal() override;
  void ShutdownInternal() override;

  /* Update loop for the physics world. Provide the delta time frame which is fixed. Called once every update frame */
  void Update(float dt);

  void Reload();

  /* Clean Up our physics world */
  void CleanUp();

  /* Clear dispatcher events */
  void ClearDispatcher();

  /* Reset our physics world back to initial state */
  void Reset();

  /* Create rigid body in the physics world */
  RigidBody* CreateRigidBody(const Entity& entity);

  /* Destroy rigid body in the physics world */
  void DestroyRigidBody(RigidBody* body);

  /* Create collider in the rigidbody */
  //ICollider* CreateCollider(IRigidBody* body, ColliderType type, size_t id) override;

  /* Create mesh collider in the rigidbody, referencing a mesh model */
  //ICollider* CreateCollider(IRigidBody* body, ColliderType type, size_t id, uint32_t meshId) override;

  /* Destroy collider from rigid body in the physics world */
  //void DestroyCollider(IRigidBody* body, ICollider* collider) override;

  /* Test a Raycast with given input */
  //bool RayCast(const Vec3& point1, const Vec3& direction, RayCastOutput& output, float distance = FLT_MAX, uint32_t layerMask = BP_ALL_LAYER, TriggerInteraction queryTriggerInteraction = eTI_UseGlobal) const override;

  /* Test a Sphere cast with given input */
  //bool SphereCast(const Vec3& origin, float radius, const Vec3& direction, RayCastOutput& output, float distance = FLT_MAX, uint32_t layerMask = BP_ALL_LAYER, TriggerInteraction queryTriggerInteraction = eTI_UseGlobal) const override;

  /* Test a Capsule cast with given input */
  //bool CapsuleCast(const Vec3& point1, const Vec3 point2, float radius, const Vec3& direction, RayCastOutput& output, float distance = FLT_MAX, uint32_t layerMask = BP_ALL_LAYER, TriggerInteraction queryTriggerInteraction = eTI_UseGlobal) const override;

  /* Returns a list of colliders overlap by given sphere shape */
  //Vector<ICollider*> OverlapSphere(const Vec3& position, float radius, uint32_t layerMask = BP_ALL_LAYER, TriggerInteraction queryTriggerInteraction = eTI_UseGlobal) const;

  /* Retrieves all rigidbodies in the physics world */
  container_t<UniquePtr<RigidBody>>& GetRigidBodies();

  /* Retrieve gravity of the physics world */
  //Vec3 GetGravity() const override;

  /* Retrieve collision layer */
  //CollisionLayerArray& GetCollisionLayer() override;

  /* Retrieve is paused value */
  bool GetPaused() const;

  /* Set the gravity of the physics world */
  //void SetGravity(const Vec3& gravity) override;

  /* Set the update state of the physics world */
  void SetPaused(bool paused);

  /* Set the velocity iteration for solver */
  void SetVelocityIteration(uint8_t iteration);

  /* Set the position iteration for solver */
  void SetPositionIteration(uint8_t iteration);

  /* Set the Query Hit Trigger */
  void SetQueryHitTrigger(bool hitTrigger);

  /* Private interface functions */
private:
  /* This function integrates velocities and positions */
  void Solve(float dt);

  void RebuildTree();

  friend class BPRigidBody;
  friend class BPFCollider;
  friend class BPMeshCollider;
  friend class BPMeshColliderManager;

  // Vec3                gravity_;              // Gravity of the physics world
  // CollisionLayerArray layerCollisionMatrix_; // Layer Collision Matrix. Used for special collision condition

  container_t<UniquePtr<RigidBody>> m_RigidBodies; // Stores the array of rigid bodies in the physics world

  //ContactManager contactManager_; // This manages our contacts manipulation in the physics world

  uint8_t m_VelocityIteration; // Determines how many velocity iteration to be used for solver
  uint8_t m_PositionIteration; // Determines how many position iteration to be used for solver

  bool m_IsPaused;           // Determines if the physics world is being paused
  bool m_IsNewCollider;      // Query if there's a new collider added into our physics world.
  bool m_QueriesHitTriggers; // Query if raycast should detect trigger

  //TriggerInteraction interactionMode_; // Which interaction mode to be used for raycast

  //Vector<RayCastDebug> rayCastDebug_; // Vector of RayCast Debug information
  //Vector<OverlapShapeOutput> overlapShapeDebug_;
  // UnorderedMap<GJK2::GJKOutput> GJKDebug_;     // Vector of GJK debug information

  //BPMeshCooker meshCooker_; // Mesh cooker system which generate bounding convex polyhedron given vertices and indices informations

  //BPMeshColliderManager meshColliderManager_; // Manage the mesh colliders (convex/non-convex) generated.
};