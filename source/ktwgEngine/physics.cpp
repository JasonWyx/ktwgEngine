#include "physics.h"
#include "rigidbody.h"

Physics::Physics()
  : m_RigidBodies{}, m_VelocityIteration{12}, m_PositionIteration{6},
    m_IsPaused{ false }, m_IsNewCollider{ false }, m_QueriesHitTriggers{ true }
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
    //m_ContactManager.GenerateContactPair();
    m_IsNewCollider = false;
  }

  for (UniquePtr<RigidBody>& elem : m_RigidBodies)
    elem.get()->SynchroniseRigidBody();

  //m_ContactManager.UpdateContacts();

  // Solve(dt);
}

void Physics::Reload()
{
  // RebuildTree();
}

void Physics::CleanUp()
{
  //m_ContactManager.CleanUp();
  //ClearDispatcher();
}

void Physics::ClearDispatcher()
{
  // m_ContactManager.ClearDispatcher();
}

void Physics::Reset()
{
  //m_ContactManager.Reset();
}

RigidBody* Physics::CreateRigidBody(const Entity& entity)
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

Physics::container_t<Physics::UniquePtr<RigidBody>>& Physics::GetRigidBodies()
{
  return m_RigidBodies;
}

bool Physics::GetPaused() const
{
  return m_IsPaused;
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
    elem.get()->m_Flags &= RBF_ISLAND;

  // Clear island visited flag for contacts
  //for (auto c = m_ContactManager.m_ContactList.m_Head; c; c = c->m_Next)
  //  c->m_Flags &= ~Contact::CF_ISLAND;

  // Set up island flags
  uint32_t islandFlags = 0;
  //islandFlags |= IF_WARMSTART;
  //islandFlags |= IF_SLEEP;

  //Island island(static_cast<uint32_t>(m_RigidBodies.size()), static_cast<unsigned>(m_ContactManager.m_ContactList.m_Size));
  
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
      //island.Clear();
      uint32_t stackCount = 0;
      stack[stackCount++] = seed;
      seed->m_Flags |= RBF_ISLAND;

      while (stackCount > 0)
      {
        // Add body to island
        RigidBody* b = stack[--stackCount];
        //island.AddBody(b);

        // body in island is considered awake
        b->SetAwake(true);

        // Don't propagate static bodies
        if (b->m_Type == RBT_STATIC)
          continue;

        // For each colliders in body, we loop through each collider's edge list to determine the contact to add to island
        //for (auto& elem : b->colliders_)
        //{
        //  auto edgeList = dynamic_cast<BPFCollider*>(elem->GetInternal())->contactEdgeList_;

        //  // Get edge list and analyse the contact to determine if it needs to be in the island
        //  for (auto contactEdge = edgeList.head_; contactEdge; contactEdge = contactEdge->next_)
        //  {
        //    auto contact = contactEdge->contact_;

        //    // Skip contact already in island
        //    // Skip contact that is not colliding
        //    if (contact->flags_ & BPContact::eCFIslandFlag || !(contact->flags_ & BPContact::eCFColliding))
        //      continue;

        //    // if any of the collider is trigger collider
        //    bool triggerA = contact->pair_.colliders_[0]->GetIsTrigger();
        //    bool triggerB = contact->pair_.colliders_[1]->GetIsTrigger();

        //    // No need for collision resolution if any collider is trigger
        //    if (triggerA || triggerB)
        //      continue;

        //    // Add contact into island
        //    island.AddContact(contact);
        //    contact->flags_ |= BPContact::eCFIslandFlag;

        //    // Check the other body of the edge
        //    auto other = dynamic_cast<BPRigidBody*>(contactEdge->other_->GetRigidBody());

        //    // Other body is already on island, we skip
        //    if (other->flags_ & eBFIsland)
        //      continue;

        //    LOGASSERT(stackCount < stackCapacity, "Island stack overflow");

        //    stack[stackCount++] = other;
        //    other->flags_ |= eBFIsland;
        //  }
        //}
      }

      // Solve the island
      //island.Solve(gravity_, dt, velocityIteration_, positionIteration_, islandFlags);

      // Allow static bodies to participate in other islands.
      /*for (auto j = 0U; j < island.bodyCount_; ++j)
      {
        auto b = island.bodies_[j];
        if (b->bodyType_ == eBTStatic)
          b->flags_ &= ~eBFIsland;
      }*/
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
  //m_ContactManager.DispatchAllCollisionEvents();

  // Generate new contact pairs for next frame
  //m_ContactManager.GenerateContactPair();
}

void Physics::RebuildTree()
{
  for (UniquePtr<RigidBody>& body : m_RigidBodies)
  {
    /*for (auto& collider : body->colliders_)
    {
      if (collider->GetActive())
      {
        AABB3 aabb;
        collider->ComputeAABB(aabb, body->transform_);
        collider->SetBroadphaseId(contactManager_.broadPhase_.CreateProxy(aabb, collider));
      }
    }*/
  }
}
