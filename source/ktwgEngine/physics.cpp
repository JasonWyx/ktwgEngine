#include "physics.h"
#include "rigidbody.h"
#include "boxcollider.h"
#include "fcollider.h"
#include "raycastcallbacks.h"
#include "raycast.h"
#include "contact.h"
#include "contactmanager.h"
#include "island.h"

Physics::Physics()
  : m_Gravity{ 0.0f, -9.81f, 0.0f }, m_RigidBodies {}, m_VelocityIteration{ 12 }, m_PositionIteration{ 6 },
    m_IsPaused{ false }, m_IsNewCollider{ false }, m_QueriesHitTriggers{ true },
    m_ContactManager{}
{
  m_RigidBodies.reserve(MAX_RIGIDBODIES);
}

Physics::~Physics()
{
}

void Physics::InitializeInternal()
{
}

void Physics::ShutdownInternal()
{
}

void Physics::Update(float dt)
{
  if (m_IsPaused)
    return;

  if (m_IsNewCollider)
  {
    m_ContactManager.GenerateContactPair();
    m_IsNewCollider = false;
  }

  for (UniquePtr<RigidBody>& elem : m_RigidBodies)
    elem.get()->SynchroniseRigidBody();

  m_ContactManager.UpdateContacts();

  Solve(dt);
}

void Physics::Reload()
{
  RebuildTree();
}

void Physics::CleanUp()
{
  m_ContactManager.CleanUp();
  ClearDispatcher();
}

void Physics::ClearDispatcher()
{
  m_ContactManager.ClearDispatcher();
}

void Physics::Reset()
{
  m_ContactManager.Reset();
}

RigidBody* Physics::CreateRigidBody(Entity& entity)
{
  m_RigidBodies.emplace_back(std::make_unique<RigidBody>(entity));
  return m_RigidBodies.back().get();
}

void Physics::DestroyRigidBody(RigidBody* body)
{
  for (int i = 0; i < m_RigidBodies.size(); ++i)
  {
    if (m_RigidBodies[i].get() == body)
    {
      m_RigidBodies[i].release();
      m_RigidBodies.erase(m_RigidBodies.begin() + i);
      break;
    }
  }
}

BoxCollider* Physics::CreateCollider(RigidBody* body, uint32_t id)
{
  BoxCollider* collider = body->CreateCollider(id);
  return collider;
}

void Physics::DestroyCollider(RigidBody* body, BoxCollider* collider)
{
  body->DestroyCollider(collider);
}

bool Physics::RayCast(const Vec3& point1, const Vec3& direction, RayCastOutput& output, float distance, uint32_t layerMask, TriggerInteraction queryTriggerInteraction) const
{
  // Set up the ray cast input
  RayCastInput input;
  input.m_p = point1;

  // Make sure direction is normalized
  auto dir = Normalize(direction);

  // Calculate the end point
  input.m_q = point1 + distance * dir;

  input.m_MaxT = 1.0f;
  input.m_LayerMask = layerMask;

  output.Reset();

  RayCastCallback* rcCallBack = new RayCastCallback{};
  rcCallBack->broadPhase_ = &m_ContactManager.broadPhase_;
  rcCallBack->output_ = &output;


  switch (queryTriggerInteraction)
  {
  case TI_USEGLOBAL:
    m_ContactManager.broadPhase_.RayCast(rcCallBack, input, m_QueriesHitTriggers);
    break;
  case TI_IGNORE:
    m_ContactManager.broadPhase_.RayCast(rcCallBack, input, false);
    break;
  case TI_COLLIDE:
    m_ContactManager.broadPhase_.RayCast(rcCallBack, input, true);
    break;
  }
  if (output.m_Collider)
  {
    output.m_Point = (1.0f - output.m_T) * input.m_p + output.m_T * input.m_q;

    delete rcCallBack;
    return true;
  }
  delete rcCallBack;
  return false;
}

Physics::container_t<Physics::UniquePtr<RigidBody>>& Physics::GetRigidBodies()
{
  return m_RigidBodies;
}

Vec3 Physics::GetGravity() const
{
  return m_Gravity;
}

Physics::CollisionLayerArray& Physics::GetCollisionLayer()
{
  return m_LayerCollisionMatrix;
}

bool Physics::GetPaused() const
{
  return m_IsPaused;
}

void Physics::SetGravity(const Vec3& gravity)
{
  m_Gravity = gravity;
}

void Physics::SetPaused(bool paused)
{
  m_IsPaused = paused;
}

void Physics::SetVelocityIteration(uint8_t iteration)
{
  m_VelocityIteration = iteration;
}

void Physics::SetPositionIteration(uint8_t iteration)
{
  m_PositionIteration = iteration;
}

void Physics::SetQueryHitTrigger(bool hitTrigger)
{
  m_QueriesHitTriggers = hitTrigger;
}

void Physics::Solve(float dt)
{
  // Clear island visited flag for bodies to do DFS
  for (UniquePtr<RigidBody>& elem : m_RigidBodies)
    elem.get()->m_Flags &= ~RBF_ISLAND;

  // Clear island visited flag for contacts
  for (auto c = m_ContactManager.contactList_.head_; c; c = c->next_)
    c->flags_ &= ~Contact::CF_ISLANDFLAG;

  // Set up island flags
  uint32_t islandFlags = 0;
  islandFlags |= IF_WARMSTART;
  islandFlags |= IF_SLEEP;

  Island island(static_cast<uint32_t>(m_RigidBodies.size()), static_cast<unsigned>(m_ContactManager.contactList_.size_));
  
  // Build and simulate awake island
  uint32_t stackCapacity = (uint32_t)m_RigidBodies.size();

  RigidBody** stack = new RigidBody*[stackCapacity];
  {

    for (int i = 0; i < m_RigidBodies.size(); ++i)
    {
      RigidBody* seed = m_RigidBodies[i].get();

      // Skip inactive body
      if (!seed->m_Active)
        continue;

      // Skip seed that is already on an island
      // Skip seed that is sleeping
      // Skip seed that is static
      if ((seed->m_Flags & RBF_ISLAND) || !(seed->m_Flags & RBF_AWAKE) || (seed->m_Type == RBT_STATIC))
        continue;

      // Perform DFS on body constraint graph
      island.Clear();
      uint32_t stackCount = 0;
      stack[stackCount++] = seed;
      seed->m_Flags |= RBF_ISLAND;

      while (stackCount > 0)
      {
        // Add body to island
        RigidBody* b = stack[--stackCount];
        island.AddBody(b);

        // body in island is considered awake
        b->SetAwake(true);

        // Don't propagate static bodies
        if (b->m_Type == RBT_STATIC)
          continue;

        // For each colliders in body, we loop through each collider's edge list to determine the contact to add to island
        for (auto& elem : b->m_Colliders)
        {
          auto edgeList = elem->GetInternal()->m_ContactEdgeList;

          // Get edge list and analyse the contact to determine if it needs to be in the island
          for (auto contactEdge = edgeList.head_; contactEdge; contactEdge = contactEdge->next_)
          {
            auto contact = contactEdge->contact_;

            // Skip contact already in island
            // Skip contact that is not colliding
            if (contact->flags_ & Contact::CF_ISLANDFLAG || !(contact->flags_ & Contact::CF_COLLIDING))
              continue;

            // if any of the collider is trigger collider
            bool triggerA = contact->pair_.colliders_[0]->GetIsTrigger();
            bool triggerB = contact->pair_.colliders_[1]->GetIsTrigger();

            // No need for collision resolution if any collider is trigger
            if (triggerA || triggerB)
              continue;

            // Add contact into island
            island.AddContact(contact);
            contact->flags_ |= Contact::CF_ISLANDFLAG;

            // Check the other body of the edge
            auto other = contactEdge->other_->GetRigidBody();

            // Other body is already on island, we skip
            if (other->m_Flags & RBF_ISLAND)
              continue;

            assert(stackCount < stackCapacity);

            stack[stackCount++] = other;
            other->m_Flags |= RBF_ISLAND;
          }
        }
      }

      // Solve the island
      island.Solve(m_Gravity, dt, m_VelocityIteration, m_PositionIteration, islandFlags);

      // Allow static bodies to participate in other islands.
      for (auto j = 0U; j < island.bodyCount_; ++j)
      {
        auto b = island.bodies_[j];
        if (b->m_Type == RBT_STATIC)
          b->m_Flags &= RBF_ISLAND;
      }
    }
  }

  delete[] stack;

  for (UniquePtr<RigidBody>&elem : m_RigidBodies)
  {
    // If a body didn't participate on a island then it didn't move.
    if (!(elem->m_Flags & RBF_ISLAND))
      continue;

    if (elem->m_Type == RBT_STATIC)
      continue;

    elem->SynchroniseProxies();
  }

  // Dispatch our collision events that is stored during our check
  m_ContactManager.DispatchAllCollisionEvents();

  // Generate new contact pairs for next frame
  m_ContactManager.GenerateContactPair();
}

void Physics::RebuildTree()
{
  for (UniquePtr<RigidBody>& body : m_RigidBodies)
  {
    for (auto& collider : body->m_Colliders)
    {
      if (collider->GetActive())
      {
        AABB3 aabb;
        collider->ComputeAABB(aabb, body->m_Transform);
        collider->SetBroadphaseId(m_ContactManager.broadPhase_.CreateProxy(aabb, collider.get()));
      }
    }
  }
}
