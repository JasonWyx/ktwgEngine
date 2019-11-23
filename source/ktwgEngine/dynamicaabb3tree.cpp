#include "dynamicaabb3tree.h"
#include <memory>

#define Max(a, b) a > b ? a : b

DynamicAABB3Tree::DynamicAABB3Tree()
  : nodes_{ nullptr }, capacity_{ TREE_CAPACITY }, count_{ 0 }, freeList_{ 0 }, root_{ NULL_NODE }
{
  // Allocate a pool of tree nodes for usage. Increase the tree capacity if needed
  nodes_ = new TreeNode[capacity_];

  // Build our free list from 0
  AddToFreeList(count_);
}

DynamicAABB3Tree::~DynamicAABB3Tree()
{
  delete[] nodes_;
}

int32_t DynamicAABB3Tree::InsertNode(const AABB3& aabb, void* userData)
{
  // Allocate from our pool a new node
  auto node = AllocateNode();

  // Assign our data to the tree node
  nodes_[node].fatAABB_  = aabb;
  nodes_[node].userData_ = userData;
  nodes_[node].height_   = 0;

  // Insert our node as a leaf node
  InsertLeafNode(node);

  // Return the node index that we insert to
  return node;
}

void DynamicAABB3Tree::RemoveNode(int32_t index)
{
  assert(index >= 0 || index < capacity_);
  assert(nodes_[index].IsLeaf());

  // Remove our leaf node from the tree first
  RemoveLeafNode(index);

  // Deallocate the node
  DeallocateNode(index);
}

void DynamicAABB3Tree::UpdateNode(int32_t index, const AABB3& aabb)
{
  RemoveLeafNode(index);
  nodes_[index].fatAABB_ = aabb;
  InsertLeafNode(index);
}

const AABB3& DynamicAABB3Tree::GetFatAABB(int32_t index) const
{
  assert(index != NULL_NODE && index < capacity_);
  return nodes_[index].fatAABB_;
}

void* DynamicAABB3Tree::GetUserData(int32_t index) const
{
  assert(index != NULL_NODE && index < capacity_);
  return nodes_[index].userData_;
}

int32_t DynamicAABB3Tree::GetRoot() const
{
  return root_;
}

TreeNode* DynamicAABB3Tree::GetTreeNodes() const
{
  return nodes_;
}

void DynamicAABB3Tree::Reset()
{
  // Reset our tree
  count_ = 0;
  root_ = NULL_NODE;

  // Rebuild our free list from 0
  AddToFreeList(count_);
}

void DynamicAABB3Tree::AddToFreeList(int32_t index)
{
  // Build the link list starting from given node index
  for (auto i = index; i < capacity_ - 1; ++i)
  {
    nodes_[i].next_ = i + 1;
    nodes_[i].height_ = -1;
  }

  // last node must be link to a nullptr
  nodes_[capacity_ - 1].next_ = NULL_NODE;
  nodes_[capacity_ - 1].height_ = -1;

  freeList_ = index;
}

int32_t DynamicAABB3Tree::AllocateNode()
{
  // If there's not enough nodes in memory pool
  if (freeList_ == NULL_NODE)
  {
    // Double the cap, allocate a bigger pool
    // Copy the old pool to new pool
    // Free our old pool and grab the new pool
    auto old_capacity = capacity_;
    auto old_nodes = nodes_;
    capacity_ *= 2;
    nodes_ = new TreeNode[capacity_];
    std::memcpy(nodes_, old_nodes, count_ * sizeof(TreeNode));
    delete[] old_nodes;

    // Link the free list's nodes starting from our count
    AddToFreeList(count_);
  }

  // Grab an index of a free node
  auto new_node = freeList_;

  // Update our free list
  freeList_ = nodes_[new_node].next_;

  // Initialize our node to prepare it for usage
  nodes_[new_node].Initialize();

  // Increment our counter
  ++count_;

  return new_node;
}

void DynamicAABB3Tree::DeallocateNode(int32_t index)
{
  // Connect our node back to our free list
  nodes_[index].next_ = freeList_;
  nodes_[index].height_ = -1;
  nodes_[index].userData_ = nullptr;
  freeList_ = index;
  --count_;
}

void DynamicAABB3Tree::InsertLeafNode(int32_t index)
{
  // When tree is empty case
  // Insert it as our root node
  if (root_ == NULL_NODE)
  {
    // A root node has no parent
    root_ = index;
    nodes_[root_].parent_ = NULL_NODE;
    return;
  }

  // Get the AABB from inserted node
  auto aabb = nodes_[index].fatAABB_;

  // Search for a sibling to form a branch with
  auto sibling = FindBestSibling(aabb);

  // Create a new parent to form the branch with sibling
  auto old_parent = nodes_[sibling].parent_;
  auto new_parent = AllocateNode();
  nodes_[new_parent].parent_ = old_parent;
  nodes_[new_parent].fatAABB_ = Combine(nodes_[sibling].fatAABB_, aabb);
  nodes_[new_parent].height_ = nodes_[sibling].height_ + 1;
  nodes_[new_parent].userData_ = nullptr;
  nodes_[new_parent].children_[0] = sibling;
  nodes_[new_parent].children_[1] = index;
  nodes_[sibling].parent_ = new_parent;
  nodes_[index].parent_ = new_parent;

  // If the sibling is a root node, our new root is the created node (branch node)
  if (old_parent == NULL_NODE)
    root_ = new_parent;
  else
  {
    // Sibling is not a root node, find if left/right is the sibling node and link our new
    // parent node to to old parent
    if (nodes_[old_parent].children_[0] == sibling)
      nodes_[old_parent].children_[0] = new_parent;
    else
      nodes_[old_parent].children_[1] = new_parent;
  }

  // We need to perform a traversal back up and update all the AABBs above
  UpdateTreeHierarchy(new_parent);
}

void DynamicAABB3Tree::RemoveLeafNode(int32_t index)
{
  // If leaf node is a root node
  if (index == root_)
  {
    root_ = NULL_NODE;
    return;
  }

  auto parent = nodes_[index].parent_;
  auto grandparent = nodes_[parent].parent_;
  int32_t sibling; // Need to get the sibling of the node to remove
  if (nodes_[parent].children_[0] == index)
    sibling = nodes_[parent].children_[1];
  else
    sibling = nodes_[parent].children_[0];

  // If grandparent is not a root node
  // Need to remove parent from grandparent and replace it with sibling
  if (grandparent != NULL_NODE)
  {
    if (nodes_[grandparent].children_[0] == parent)
      nodes_[grandparent].children_[0] = sibling;
    else
      nodes_[grandparent].children_[1] = sibling;

    nodes_[sibling].parent_ = grandparent;

    DeallocateNode(parent);
    UpdateTreeHierarchy(grandparent);
  }
  else
  {
    // parent is a root node
    root_ = sibling;
    nodes_[sibling].parent_ = NULL_NODE;
    DeallocateNode(parent);
  }
}

int32_t DynamicAABB3Tree::FindBestSibling(const AABB3& aabb)
{
  // Start our search from the root of our tree
  auto current = root_;

  // Traverse the whole tree to compute heuristic cost and find the best sibling
  // to form a branch with
  while (!nodes_[current].IsLeaf())
  {
    auto branch_area = nodes_[current].fatAABB_.SurfaceArea();

    // Combine the aabbs to calculate surface area
    auto combined_AABB = Combine(nodes_[current].fatAABB_, aabb);
    auto combined_area = combined_AABB.SurfaceArea();

    // Calculate the heuristic cost
    auto branch_cost = 2.0f * combined_area;
    auto inheritance_cost = 2.0f * (combined_area - branch_area);

    // Check the childrens
    auto left_child = nodes_[current].children_[0];
    auto right_child = nodes_[current].children_[1];

    // Compute the cost to descent to the left child branch node
    auto left_descent_cost = 0.0f;
    if (nodes_[left_child].IsLeaf())
      left_descent_cost = Combine(nodes_[left_child].fatAABB_, aabb).SurfaceArea();
    else
    {
      auto old_area = nodes_[left_child].fatAABB_.SurfaceArea();
      auto new_area = Combine(nodes_[left_child].fatAABB_, aabb).SurfaceArea();
      left_descent_cost = (new_area - old_area) + inheritance_cost;
    }

    // Compute the cost to descent to the right child branch node
    auto right_descent_cost = 0.0f;
    if (nodes_[right_child].IsLeaf())
      right_descent_cost = Combine(nodes_[right_child].fatAABB_, aabb).SurfaceArea();
    else
    {
      auto old_area = nodes_[right_child].fatAABB_.SurfaceArea();
      auto new_area = Combine(nodes_[right_child].fatAABB_, aabb).SurfaceArea();
      right_descent_cost = (new_area - old_area) + inheritance_cost;
    }

    // If current branch node is the better one, we break from the traversal and use branch node
    if (branch_cost < left_descent_cost && branch_cost < right_descent_cost)
      break;

    // Determine to traverse left/right based on heuristic descent cost
    current = left_descent_cost < right_descent_cost ? left_child : right_child;
  }

  return current;
}

void DynamicAABB3Tree::UpdateTreeHierarchy(int32_t index)
{
  // Traverse the tree upwards from given index and combine all AABB
  // to update the bounding volumes
  while (index != NULL_NODE)
  {
    // index = Balance(index);

    auto left_child = nodes_[index].children_[0];
    auto right_child = nodes_[index].children_[1];

    nodes_[index].height_ = 1 + Max(nodes_[left_child].height_, nodes_[right_child].height_);
    nodes_[index].fatAABB_ = Combine(nodes_[left_child].fatAABB_, nodes_[right_child].fatAABB_);

    index = nodes_[index].parent_;
  }
}

int32_t DynamicAABB3Tree::Balance(int32_t iA)
{
  // Perform AVL tree balancing based on the given node
  // Perform rotation to get our tree to be balanced for better traversal performance

  // Balanced tree
  /*     A
      /     \
     B       C
    / \     / \
   D   E   F   G
  */

  auto node_A = nodes_ + iA;

  // Tree is a leaf/ tree height is one, no balancing required
  if (node_A->IsLeaf() || node_A->height_ < 2)
    return iA;

  auto iB = node_A->children_[0];
  auto iC = node_A->children_[1];

  auto node_B = nodes_ + iB;
  auto node_C = nodes_ + iC;

  // See which side requires balancing by querying the height differences
  auto balance = node_C->height_ - node_B->height_;

  // C height is larger, we need to rotate left
  /*     A                            C
      /     \                       /    \
     B       C                     A     D/E ? (based on height)
          /     \       ==>       / \
         D       E               B  D/E ? (based on height)
        / \      /\
       F   G    H  I
  */
  if (balance > 1)
  {
    auto iD = node_C->children_[0];
    auto iE = node_C->children_[1];

    auto node_D = nodes_ + iD;
    auto node_E = nodes_ + iE;

    // Make C a parent of A, A is left child
    node_C->children_[0] = iA;
    node_C->parent_ = node_A->parent_;
    node_A->parent_ = iC;

    // If node A was a root node, we need to update our new root to C
    if (node_C->parent_ == NULL_NODE)
      root_ = iC;
    else
    {
      // Update A's old parent to have C as children
      // is node A a left child or right child previously?
      if (nodes_[node_C->parent_].children_[0] == iA)
        nodes_[node_C->parent_].children_[0] = iC;
      else
        nodes_[node_C->parent_].children_[1] = iC;
    }

    // Perform rotation of node C's child
    // Decide on which children goes to A or C based on height
    if (node_D->height_ > node_E->height_)
    {
      node_C->children_[1] = iD;
      node_A->children_[1] = iE;
      node_E->parent_ = iA;
      node_A->fatAABB_ = Combine(node_B->fatAABB_, node_E->fatAABB_);
      node_C->fatAABB_ = Combine(node_A->fatAABB_, node_D->fatAABB_);
      node_A->height_ = 1 + Max(node_B->height_, node_E->height_);
      node_C->height_ = 1 + Max(node_A->height_, node_D->height_);
    }
    else
    {
      node_C->children_[1] = iE;
      node_A->children_[1] = iD;
      node_D->parent_ = iA;
      node_A->fatAABB_ = Combine(node_B->fatAABB_, node_D->fatAABB_);
      node_C->fatAABB_ = Combine(node_A->fatAABB_, node_E->fatAABB_);
      node_A->height_ = 1 + Max(node_B->height_, node_D->height_);
      node_C->height_ = 1 + Max(node_A->height_, node_E->height_);
    }

    // iC is now the index that replaces iA
    return iC;
  }

  // B height is larger, we need to rotate right
  /*          A                            B
           /     \                       /    \
          B       C                    D/E?    A
        /   \             ==>                 /  \
       D     E                              D/E?  C
      / \    /\
     F   G  H  I
  */
  if (balance < -1)
  {
    auto iD = node_B->children_[0];
    auto iE = node_B->children_[1];

    auto node_D = nodes_ + iD;
    auto node_E = nodes_ + iE;

    // Make B a parent of A, A is right child
    node_B->children_[0] = iA;
    node_B->parent_ = node_A->parent_;
    node_A->parent_ = iB;

    // If node A was a root node, we need to update our new root to B
    if (node_B->parent_ == NULL_NODE)
      root_ = iB;
    else
    {
      // Update A's old parent to have B as children
      // is node A a left child or right child previously?
      if (nodes_[node_B->parent_].children_[0] == iA)
        nodes_[node_B->parent_].children_[0] = iB;
      else
        nodes_[node_B->parent_].children_[1] = iB;
    }

    // Perform rotation of node B's child
    // Decide on which children goes to A or B based on height
    if (node_D->height_ > node_E->height_)
    {
      node_B->children_[1] = iD;
      node_A->children_[0] = iE;
      node_E->parent_ = iA;
      node_A->fatAABB_ = Combine(node_C->fatAABB_, node_E->fatAABB_);
      node_B->fatAABB_ = Combine(node_A->fatAABB_, node_D->fatAABB_);
      node_A->height_ = 1 + Max(node_C->height_, node_E->height_);
      node_B->height_ = 1 + Max(node_A->height_, node_D->height_);
    }
    else
    {
      node_B->children_[1] = iE;
      node_A->children_[0] = iD;
      node_D->parent_ = iA;
      node_A->fatAABB_ = Combine(node_C->fatAABB_, node_D->fatAABB_);
      node_B->fatAABB_ = Combine(node_A->fatAABB_, node_E->fatAABB_);
      node_A->height_ = 1 + Max(node_C->height_, node_D->height_);
      node_B->height_ = 1 + Max(node_A->height_, node_E->height_);
    }

    // iB is now the index that replaces iA
    return iB;
  }

  return iA;
}
