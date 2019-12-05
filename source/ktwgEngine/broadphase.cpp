#include "broadphase.h"
#include "physicsconstants.h"
#include "boxcollider.h"
#include "contactmanager.h"
#include "raycastcallbacks.h"

BroadPhase::BroadPhase(ContactManager* manager)
  : contactManager_{ manager }, 
    pairsBuffer_{ nullptr }, 
    pairCount_{ 0 }, 
    pairCapacity_{ PAIR_CAPACITY },
    moveBuffer_{ nullptr }, 
    moveBufferCount_{ 0 }, 
    moveBufferCapacity_{ MOVE_BUFFER_CAPACITY }
{
  // Allocate a pair buffer once
  pairsBuffer_ = new ProxyPair[pairCapacity_];
  moveBuffer_ = new int32_t[moveBufferCapacity_];
}

BroadPhase::~BroadPhase()
{
  delete[] pairsBuffer_;
  delete[] moveBuffer_;
}

void BroadPhase::GeneratePairs()
{
  // Clear our pair buffer and prepare to generate new pairs of contacts
  pairCount_ = 0;

  // Perform queries of the tree with all moved proxies
  for (int32_t i = 0; i < moveBufferCount_; ++i)
  {
    queryId_ = moveBuffer_[i];

    if (queryId_ == NULL_NODE)
      continue;

    const AABB3& fatAABB = tree_.GetFatAABB(queryId_);
    tree_.QueryAABB(this, fatAABB);
  }

  // Reset move count
  moveBufferCount_ = 0;

  // Sort the pair buffer to expose all duplicates
  std::sort(pairsBuffer_, pairsBuffer_ + pairCount_, ProxyPairCompare);

  // Send the pairs back to the client.
  int32_t i = 0;
  while (i < (int32_t)pairCount_)
  {
    ProxyPair* primaryPair = pairsBuffer_ + i;
    BoxCollider* colliderA = reinterpret_cast<BoxCollider*>(tree_.GetUserData(primaryPair->proxyA));
    BoxCollider* colliderB = reinterpret_cast<BoxCollider*>(tree_.GetUserData(primaryPair->proxyB));

    if (!colliderA || !colliderB || !colliderA->GetInternal() || !colliderB->GetInternal())
    {
      ++i;
      continue;
    }

    if (!colliderA->GetActive() || !colliderB->GetActive())
    {
      ++i;
      continue;
    }

    contactManager_->AddContactPair(colliderA, colliderB);
    ++i;

    // Skip any duplicate pairs.
    while (i < (int32_t)pairCount_)
    {
      auto pair = pairsBuffer_ + i;

      if (pair->proxyA != primaryPair->proxyA || pair->proxyB != primaryPair->proxyB)
        break;

      ++i;
    }
  }
}

int32_t BroadPhase::CreateProxy(const AABB3& aabb, void* data)
{
  // Given a tight-fitting AABB
  // Fatten it and place it into our AABB tree
  auto fatAABB = aabb;
  fatAABB.Extend(AABB_EXTENSION);

  auto id = tree_.InsertNode(fatAABB, data);
  BufferMove(id);
  return id;
}

void BroadPhase::DestroyProxy(int32_t index)
{
  tree_.RemoveNode(index);
}

bool BroadPhase::MoveProxy(int32_t index, const AABB3& aabb, const Vec3& displacement)
{
  if (index == -1)
    return false;

  // New aabb is contained in old AABB
  if (tree_.GetFatAABB(index).Contains(aabb))
    return false;

  const Vec3 extension(AABB_EXTENSION, AABB_EXTENSION, AABB_EXTENSION);

  AABB3 fatAABB;
  fatAABB.m_Min = aabb.m_Min - extension;
  fatAABB.m_Max = aabb.m_Max + extension;

  if (displacement.x_ < 0.0f)
    fatAABB.m_Min.x_ += AABB_MULTIPLIER * displacement.x_;
  else
    fatAABB.m_Max.x_ += AABB_MULTIPLIER * displacement.x_;

  if (displacement.y_ < 0.0f)
    fatAABB.m_Min.y_ += AABB_MULTIPLIER * displacement.y_;
  else
    fatAABB.m_Max.y_ += AABB_MULTIPLIER * displacement.y_;

  if (displacement.z_ < 0.0f)
    fatAABB.m_Min.z_ += AABB_MULTIPLIER * displacement.z_;
  else
    fatAABB.m_Max.z_ += AABB_MULTIPLIER * displacement.z_;

  tree_.UpdateNode(index, fatAABB);

  BufferMove(index);

  return true;
}

void* BroadPhase::GetUserData(int32_t index) const
{
  return tree_.GetUserData(index);
}

bool BroadPhase::TestOverlap(int32_t A, int32_t B) const
{
  return AABB3toAABB3(tree_.GetFatAABB(A), tree_.GetFatAABB(B));
}

void BroadPhase::RayCast(CastCallback* callback, const RayCastInput& input, bool hitTriggers) const
{
  tree_.RayCast(callback, input, hitTriggers);
}

int32_t BroadPhase::GetTreeRoot() const
{
  return tree_.GetRoot();
}

TreeNode* BroadPhase::GetTreeNodes() const
{
  return tree_.GetTreeNodes();
}

void BroadPhase::Reset()
{
  tree_.Reset();
  moveBufferCount_ = 0;
}

void BroadPhase::BufferMove(int32_t id)
{
  // If our buffer is full, we expand the capacity
  if (moveBufferCount_ == moveBufferCapacity_)
  {
    // Double our capacity
    auto old_capacity = moveBufferCapacity_;
    moveBufferCapacity_ *= 2;

    // Copy old to new buffer
    // And replace our buffer with the new one
    auto oldMoveBuffer = moveBuffer_;
    moveBuffer_ = new int32_t[moveBufferCapacity_];
    std::memcpy(moveBuffer_, oldMoveBuffer, moveBufferCount_ * sizeof(int32_t));
    delete[] oldMoveBuffer;
  }

  moveBuffer_[moveBufferCount_] = id;
  ++moveBufferCount_;
}

bool BroadPhase::QueryCallback(int32_t id)
{
  // Proxy cannot overlap with itself
  if (id == queryId_)
    return true;

  // Check capacity, increment if needed
  if (pairCount_ == pairCapacity_)
  {
    auto oldCapacity = pairCapacity_;
    auto oldBuffer = pairsBuffer_;
    pairCapacity_ *= 2;
    pairsBuffer_ = new ProxyPair[pairCapacity_];
    memcpy(pairsBuffer_, oldBuffer, pairCount_ * sizeof(ProxyPair));
    delete[] oldBuffer;
  }

  // Build our pair list
  pairsBuffer_[pairCount_].proxyA = id < queryId_ ? id : queryId_;
  pairsBuffer_[pairCount_].proxyB = id > queryId_ ? id : queryId_;
  ++pairCount_;

  // Continue to look for overlapping pair
  return true;
}
