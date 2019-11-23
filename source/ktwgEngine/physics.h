#pragma once

#include <vector>
#include <memory>
#include "singleton.h"
#include "physicsconstants.h"
#include "contactmanager.h"
#include "triggerinteraction.h"

class Entity;
class RigidBody;
class BoxCollider;

class Physics final : public Singleton<Physics>
{
  template <typename T>
  using container_t = std::vector<T>;

  template <typename T>
  using UniquePtr = std::unique_ptr<T>;

  using CollisionLayerArray = uint32_t[MAX_COLLISION_LAYER];
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
  RigidBody* CreateRigidBody(Entity& entity);

  /* Destroy rigid body in the physics world */
  void DestroyRigidBody(RigidBody* body);

  /* Create collider in the rigidbody */
  BoxCollider* CreateCollider(RigidBody* body, uint32_t id);

  /* Destroy collider from rigid body in the physics world */
  void DestroyCollider(RigidBody* body, BoxCollider* collider);

  /* Test a Raycast with given input */
  bool RayCast(const Vec3& point1, const Vec3& direction, RayCastOutput& output, float distance = FLT_MAX, uint32_t layerMask = ALL_LAYER, TriggerInteraction queryTriggerInteraction = TI_USEGLOBAL) const;

  /* Returns a list of colliders overlap by given sphere shape */
  //Vector<ICollider*> OverlapSphere(const Vec3& position, float radius, uint32_t layerMask = BP_ALL_LAYER, TriggerInteraction queryTriggerInteraction = eTI_UseGlobal) const;

  /* Retrieves all rigidbodies in the physics world */
  container_t<UniquePtr<RigidBody>>& GetRigidBodies();

  /* Retrieve gravity of the physics world */
  Vec3 GetGravity() const;

  /* Retrieve collision layer */
  CollisionLayerArray& GetCollisionLayer();

  /* Retrieve is paused value */
  bool GetPaused() const;

  /* Set the gravity of the physics world */
  void SetGravity(const Vec3& gravity);

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

  friend class RigidBody;
  friend class FCollider;

  Vec3     m_Gravity;              // Gravity of the physics world

  CollisionLayerArray m_LayerCollisionMatrix; // Layer Collision Matrix. Used for special collision condition

  container_t<UniquePtr<RigidBody>> m_RigidBodies; // Stores the array of rigid bodies in the physics world

  ContactManager m_ContactManager; // This manages our contacts manipulation in the physics world

  uint8_t m_VelocityIteration; // Determines how many velocity iteration to be used for solver
  uint8_t m_PositionIteration; // Determines how many position iteration to be used for solver

  bool m_IsPaused;           // Determines if the physics world is being paused
  bool m_IsNewCollider;      // Query if there's a new collider added into our physics world.
  bool m_QueriesHitTriggers; // Query if raycast should detect trigger

  TriggerInteraction m_InteractionMode; // Which interaction mode to be used for raycast
};