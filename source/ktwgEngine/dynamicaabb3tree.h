#pragma once

#include <assert.h>
#include <stdint.h>
#include "aabb3.h"
#include "raycast.h"

int32_t constexpr  NULL_NODE     = -1;
int32_t constexpr  TREE_CAPACITY = 1000;

/* Represent a node in the dynamic AABB tree. */
struct TreeNode
{
  TreeNode()
    : userData_{ nullptr }, height_{}
  {
    children_[0] = children_[1] = NULL_NODE;
  }

  inline void Initialize()
  {
    children_[0] = children_[1] = parent_ = NULL_NODE;
    height_ = 0;
    userData_ = nullptr;
  }

  inline void Clear()
  {
    children_[0] = children_[1] = parent_ = NULL_NODE;
    height_ = -1;
    userData_ = nullptr;
  }

  /* Queries if this node is a leaf node */
  bool IsLeaf() const { return children_[1] == NULL_NODE; }

  AABB3 fatAABB_;     // Fat AABB to bound leafs and branches

  union
  {
    int32_t parent_;
    int32_t next_;   // Tells us the next index which is free for allocation in our free list
  };

  int32_t children_[2]; // Left node = [0], Right node = [1]

  void* userData_; // Leaf nodes holds user data, we can store both left and right indices at same memory space

  int32_t height_; // Height at the node
};

/* A dynamic AABB tree used for broadphase check in physics world. */
class DynamicAABB3Tree
{
public:
  DynamicAABB3Tree();
  ~DynamicAABB3Tree();

  /* Inserts a node into dynamic tree by providing the AABB and user data*/
  int32_t InsertNode(const AABB3& aabb, void* userData);

  /* Remove a node from dynamic tree by providing the id of the node */
  void RemoveNode(int32_t index);

  /* Update a node's AABB */
  void UpdateNode(int32_t index, const AABB3& aabb);

  /* Retrieve fat AABB */
  const AABB3& GetFatAABB(int32_t index) const;

  /* Retrieve user data */
  void* GetUserData(int32_t index) const;

  /* Used to report all overlapping AABBs with the given AABB*/
  template <typename T>
  void QueryAABB(T* callback, const AABB3& aabb) const;

  /* Used to report all ray cast intersection with the AABBs*/
  template <typename T>
  void RayCast(T* callback, const RayCastInput& input, bool hitTriggers) const;

  /* Used to report all sphere cast intersection with the AABBs*/
  template <typename T>
  void SphereCast(T* callback, const RayCastInput& input, float radius, bool hitTriggers) const;

  /* Retrieve the root of the tree */
  int32_t GetRoot() const;

  /* Retrieve the tree nodes */
  TreeNode* GetTreeNodes() const;

  /* Reset the tree back to initial state */
  void Reset();
private:
  /* Rebuild our linked list from index and update our free list */
  void AddToFreeList(int32_t index);

  /* Allocate a node from our pool, index return is the index of the node in the pool allocated */
  int32_t AllocateNode();

  /* Deallocate a node and return it to our pool of free list */
  void DeallocateNode(int32_t index);

  /* Insert a leaf node into the tree */
  void InsertLeafNode(int32_t index);

  /* Remove a leaf node from the tree */
  void RemoveLeafNode(int32_t index);

  /* Heuristic search using combined surface area to find the best sibling aabb in the tree to form branch */
  int32_t FindBestSibling(const AABB3& aabb);

  /* Updates the tree hierachy from bottom to root */
  void UpdateTreeHierarchy(int32_t index);

  /* Balances the tree from a given node */
  int32_t Balance(int32_t iA);

  TreeNode* nodes_;     // This stores the pool of free node that are pre-allocated
  int32_t   capacity_;  // Specify the capacity of the pool
  int32_t   count_;     // Size of the AABB3 Tree
  int32_t   freeList_; // Point to the head of the free list (by index)
  int32_t   root_;      // Root of this tree

};

template <typename T>
inline void DynamicAABB3Tree::QueryAABB(T* callback, const AABB3& aabb) const
{
  const auto stack_capacity = 1000;
  int32_t stack[stack_capacity];
  int32_t sp = 1;

  *stack = root_;

  // Traverse the tree to query if the given aabb has any potential collision based on the fat AABBs 
  while (sp)
  {
    assert(sp < stack_capacity);

    auto id = stack[--sp];

    if (id == NULL_NODE)
      continue;

    const TreeNode* n = nodes_ + id;

    if (AABB3toAABB3(aabb, n->fatAABB_))
    {
      // Node must be a leaf for potential collision to occur
      if (n->IsLeaf())
      {
        if (!callback->QueryCallback(id))
          return;
      }
      else
      {
        stack[sp++] = n->children_[0];
        stack[sp++] = n->children_[1];
      }
    }
  }
}

template <typename T>
inline void DynamicAABB3Tree::RayCast(T* callback, const RayCastInput& input, bool hitTriggers) const
{
  // We use a stack to traverse our tree
  const auto stack_capacity = 1000;
  int32_t stack[stack_capacity];
  int32_t sp = 1;

  *stack = root_;

  // Traverse the tree to search for all leaf node that intersects the ray
  // This will report all leaf node object that intersects with the ray, but we store the earliest intersection time
  while (sp)
  {
    assert(sp < stack_capacity);

    auto id = stack[--sp];

    // Skip null nodes
    if (id == NULL_NODE)
      continue;

    const TreeNode* n = nodes_ + id;

    // Query ray cast against bounding aabb
    if (RayToAABB3(input, n->fatAABB_))
    {
      // Only if the node is a leaf node, as our object is inside a leaf node
      if (hitTriggers)
      {
        if (n->IsLeaf())
        {
          // Perform a ray cast query with leaf node's object
          auto t = callback->Query(input, id);

          if (t == 0.0f)
            return;
        }
        else
        {
          // Add children into stack to continue the search
          stack[sp++] = n->children_[0];
          stack[sp++] = n->children_[1];
        }
      }
      else
      {
        // This tells raycast to ignore trigger colliders
        // So if query trigger returns true, leaf contains a trigger collider, don't query raycast
        if (n->IsLeaf() && !callback->QueryTriggers(id))
        {
          // Perform a ray cast query with leaf node's object
          auto t = callback->Query(input, id);

          if (t == 0.0f)
            return;
        }
        else
        {
          // Add children into stack to continue the search
          stack[sp++] = n->children_[0];
          stack[sp++] = n->children_[1];
        }
      }
    }
    else
    {
      // Add children into stack to continue the search
      stack[sp++] = n->children_[0];
      stack[sp++] = n->children_[1];
    }
  }
}

template <typename T>
inline void DynamicAABB3Tree::SphereCast(T* callback, const RayCastInput& input, float radius, bool hitTriggers) const
{
  // We use a stack to traverse our tree
  const auto stack_capacity = 1000;
  int32_t stack[stack_capacity];
  int32_t sp = 1;

  *stack = root_;

  // Traverse the tree to search for all leaf node that intersects the ray
  // This will report all leaf node object that intersects with the ray, but we store the earliest intersection time
  while (sp)
  {
    assert(sp < stack_capacity);

    auto id = stack[--sp];

    // Skip null nodes
    if (id == NULL_NODE)
      continue;

    const TreeNode* n = nodes_ + id;

    // Query sphere cast against bounding aabb
    if (RayToAABB3(input, n->fatAABB_, radius))
    {
      // Only if the node is a leaf node, as our object is inside a leaf node
      if (hitTriggers)
      {
        if (n->IsLeaf())
        {
          // Perform a ray cast query with leaf node's object
          auto t = callback->Query(input, id, radius);

          if (t == 0.0f)
            return;
        }
        else
        {
          // Add children into stack to continue the search
          stack[sp++] = n->children_[0];
          stack[sp++] = n->children_[1];
        }
      }
      else
      {
        // This tells raycast to ignore trigger colliders
        // So if query trigger returns true, leaf contains a trigger collider, don't query raycast
        if (n->IsLeaf() && !callback->QueryTriggers(id))
        {
          // Perform a ray cast query with leaf node's object
          auto t = callback->Query(input, id, radius);

          if (t == 0.0f)
            return;
        }
        else
        {
          // Add children into stack to continue the search
          stack[sp++] = n->children_[0];
          stack[sp++] = n->children_[1];
        }
      }
    }
    else
    {
      // Add children into stack to continue the search
      stack[sp++] = n->children_[0];
      stack[sp++] = n->children_[1];
    }
  }
}
