#include "gjk.h"
#include <assert.h>
#include "boxcollider.h"
#include "rigidbody.h"

void GJK::Simplex::Initialize(BoxCollider* colliderA, BoxCollider* colliderB)
{
  // Set up the simplex
  SimplexVertex* v = vertices_;

  // Get two local points from colliders
  Vec3 localPointA = colliderA->GetVertices()[0];
  Vec3 localPointB = colliderB->GetVertices()[0];

  // Retrieve transforms to convert local points to world points
  Transform xfA = colliderA->GetRigidBody()->GetTransform();
  Transform xfB = colliderB->GetRigidBody()->GetTransform();

  // Initialize the 1-simplex value 
  v->pointA_ = Multiply(xfA, localPointA);
  v->pointB_ = Multiply(xfB, localPointB);
  v->minkowskiPoint_ = v->pointB_ - v->pointA_;
  v->weight_ = 1.0f;
  v->indexA_ = 0;
  v->indexB_ = 0;

  count_ = 1;
}

void GJK::Simplex::SolveEdge()
{
  /*
               |                           |
     REGION A  |        REGION AB          |  REGION B
               A ------------------------- B

                 VORONOI REGION OF EDGE AB

    point Q can be in any of the three region
  */
  const Vec3 origin{ 0.0f, 0.0f, 0.0f }; // Origin point
  float wAB[3]; // weights of edge AB

  SimplexVertex A = vertices_[0];
  SimplexVertex B = vertices_[1];

  // Compute the barycentric weights of point A and point B with reference to origin
  ComputeBarycentricWeights(wAB, A.minkowskiPoint_, B.minkowskiPoint_, origin);

  // Closest point is A, origin point in region A (Voronoi Region)
  if (wAB[1] <= 0)
  {
    count_ = 1;
    vertices_[0] = A;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Closest point is B, origin point in region B (Voronoi Region)
  if (wAB[0] <= 0)
  {
    count_ = 1;
    vertices_[0] = B;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Origin point in region AB
  float w = 1.0f / wAB[2];
  count_ = 2;
  vertices_[0] = A;
  vertices_[0].weight_ = w * wAB[0];
  vertices_[1] = B;
  vertices_[1].weight_ = w * wAB[1];
}

void GJK::Simplex::SolveTriangle()
{
  /*
               \                  /
       REGION A \    REGION AB   /  REGION B
       --------- A ------------ B --------
                  \   REGION   /
                   \   ABC    /
                    \        /
        REGION AC    \      /     REGION BC
                      \    /
                       \  /
                        \/
                        /\
                       /  \
                      /  C \
                     /      \
                     REGION C

          VORONOI REGION OF TRIANGLE ABC

       point Q can be in any of the 7 region
*/
  const Vec3 origin{ 0.0f, 0.0f, 0.0f }; // Origin point
  float wAB[3], wBC[3], wCA[3]; // weights of edge AB and BC and CA

  auto A = vertices_[0];
  auto B = vertices_[1];
  auto C = vertices_[2];

  // Compute the barycentric weights of point A and point B with reference to origin
  ComputeBarycentricWeights(wAB, A.minkowskiPoint_, B.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wBC, B.minkowskiPoint_, C.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wCA, C.minkowskiPoint_, A.minkowskiPoint_, origin);

  // Closest point is A, origin point in region A (Vertex Region)
  if (wCA[0] <= 0 && wAB[1] <= 0)
  {
    count_ = 1;
    vertices_[0] = A;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Closest point is B, origin point in region B (Vertex Region)
  if (wAB[0] <= 0 && wBC[1] <= 0)
  {
    count_ = 1;
    vertices_[0] = B;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Closest point is C, origin point in region C (Vertex Region)
  if (wBC[0] <= 0 && wCA[1] <= 0)
  {
    count_ = 1;
    vertices_[0] = C;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Reaching here means that the point Q could be in the edge region
  // Need to check which vertex region we are in
  // To be inside the trangle ABC,
  // Q = uA + vB + wC,
  // where u + v + w = 1, 
  // 0 <= u <= 1 and 0 <= v <= 1  and 0 <= w <= 1 
  float wABC[4]; // weights of triangle ABC

  ComputeBarycentricWeights(wABC, A.minkowskiPoint_, B.minkowskiPoint_, C.minkowskiPoint_, origin);

  // If area * u/v/w <= 0.0f, this means that ABC does not form a triangle
  auto area = wABC[3];

  // Closest point lies between edge AB, origin point in region AB (Edge Region)
  if (wAB[0] > 0.0f && wAB[1] > 0.0f && (area * wABC[2] <= 0.0f))
  {
    assert(wAB[2] > 0.0f);
    count_ = 2;
    float w = 1.0f / wAB[2];
    vertices_[0] = A;
    vertices_[0].weight_ = w * wAB[0];
    vertices_[1] = B;
    vertices_[1].weight_ = w * wAB[1];
    return;
  }

  // Closest point lies between edge BC, origin point in region BC (Edge Region)
  if (wBC[0] > 0.0f && wBC[1] > 0.0f && (area * wABC[0] <= 0.0f))
  {
    assert(wBC[2] > 0.0f);
    count_ = 2;
    float w = 1.0f / wBC[2];
    vertices_[0] = B;
    vertices_[0].weight_ = w * wBC[0];
    vertices_[1] = C;
    vertices_[1].weight_ = w * wBC[1];
    return;
  }

  // Closest point lies between edge CA, origin point in region CA (Edge Region)
  if (wCA[0] > 0.0f && wCA[1] > 0.0f && (area * wABC[1] <= 0.0f))
  {
    assert(wCA[2] > 0.0f);
    count_ = 2;
    auto w = 1.0f / wCA[2];
    vertices_[0] = C;
    vertices_[0].weight_ = w * wCA[0];
    vertices_[1] = A;
    vertices_[1].weight_ = w * wCA[1];
    return;
  }

  if (area <= 0.0f)
    return;

  // Closest point lies in region ABC (Triangle Region)
  count_ = 3;
  auto w = 1.0f / area;
  vertices_[0] = A;
  vertices_[0].weight_ = w * wABC[0];
  vertices_[1] = B;
  vertices_[1].weight_ = w * wABC[1];
  vertices_[2] = C;
  vertices_[2].weight_ = w * wABC[2];
}

void GJK::Simplex::SolveTetrahedron()
{
  // Origin point to check 
  const Vec3 origin{};

  // Vertices of tetrahedron
  SimplexVertex A = vertices_[0];
  SimplexVertex B = vertices_[1];
  SimplexVertex C = vertices_[2];
  SimplexVertex D = vertices_[3];

  // Weights of all edges
  float wAB[3], wAC[3], wAD[3], wBC[3], wCD[3], wDB[3];

  // Compute all weight edges
  ComputeBarycentricWeights(wAB, A.minkowskiPoint_, B.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wAC, A.minkowskiPoint_, C.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wAD, A.minkowskiPoint_, D.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wBC, B.minkowskiPoint_, C.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wCD, C.minkowskiPoint_, D.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wDB, D.minkowskiPoint_, B.minkowskiPoint_, origin);

  // Vertex region test

  // Closest point is point A, origin point in region A
  if (wAB[1] <= 0.0f && wAC[1] <= 0.0f && wAD[1] <= 0.0f)
  {
    count_ = 1;
    vertices_[0] = A;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Closest point is point B, origin point in region B
  if (wAB[0] <= 0.0f && wBC[1] <= 0.0f && wDB[0] <= 0.0f)
  {
    count_ = 1;
    vertices_[0] = B;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Closest point is point C, origin point in region C
  if (wAC[0] <= 0.0f && wBC[0] <= 0.0f && wCD[1] <= 0.0f)
  {
    count_ = 1;
    vertices_[0] = C;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Closest point is point D, origin point in region D
  if (wAD[0] <= 0.0f && wCD[0] <= 0.0f && wDB[1] <= 0.0f)
  {
    count_ = 1;
    vertices_[0] = D;
    vertices_[0].weight_ = 1.0f;
    return;
  }

  // Edge Region Test

  // Weights of all triangle faces
  float wACB[4], wADC[4], wABD[4], wBCD[4];

  ComputeBarycentricWeights(wACB, A.minkowskiPoint_, C.minkowskiPoint_, B.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wADC, A.minkowskiPoint_, D.minkowskiPoint_, C.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wABD, A.minkowskiPoint_, B.minkowskiPoint_, D.minkowskiPoint_, origin);
  ComputeBarycentricWeights(wBCD, B.minkowskiPoint_, C.minkowskiPoint_, D.minkowskiPoint_, origin);

  // Closest point lies on edge AB, origin point in region AB
  if (wAB[0] > 0.0f && wAB[1] > 0.0f && wACB[1] <= 0.0f && wABD[2])
  {
    float w = 1.0f / wAB[2];
    count_ = 2;
    vertices_[0] = A;
    vertices_[0].weight_ = w * wAB[0];
    vertices_[1] = B;
    vertices_[1].weight_ = w * wAB[1];
    return;
  }

  // Closest point lies on edge AC, origin point in region AC
  if (wAC[0] > 0.0f && wAC[1] > 0.0f && wACB[2] <= 0.0f && wADC[1] <= 0.0f)
  {
    float w = 1.0f / wAC[2];
    count_ = 2;
    vertices_[0] = A;
    vertices_[0].weight_ = w * wAC[0];
    vertices_[1] = C;
    vertices_[1].weight_ = w * wAC[1];
    return;
  }

  // Closest point lies on edge AD, origin point in region AD
  if (wAD[0] > 0.0f && wAD[1] > 0.0f && wADC[2] <= 0.0f && wABD[1] <= 0.0f)
  {
    float w = 1.0f / wAD[2];
    count_ = 2;
    vertices_[0] = A;
    vertices_[0].weight_ = w * wAD[0];
    vertices_[1] = D;
    vertices_[1].weight_ = w * wAD[1];
    return;
  }

  // Closest point lies on edge BC, origin point in region BC
  if (wBC[0] > 0.0f && wBC[1] > 0.0f && wACB[0] <= 0.0f && wBCD[2] <= 0.0f)
  {
    float w = 1.0f / wBC[2];
    count_ = 2;
    vertices_[0] = B;
    vertices_[0].weight_ = w * wBC[0];
    vertices_[1] = C;
    vertices_[1].weight_ = w * wBC[1];
    return;
  }

  // Closest point lies on edge CD, origin point in region CD
  if (wCD[0] > 0.0f && wCD[1] > 0.0f && wADC[0] <= 0.0f && wBCD[0] <= 0.0f)
  {
    float w = 1.0f / wCD[2];
    count_ = 2;
    vertices_[0] = C;
    vertices_[0].weight_ = w * wCD[0];
    vertices_[1] = D;
    vertices_[1].weight_ = w * wCD[1];
    return;
  }

  // Closest point lies on edge DB, origin point in region DB
  if (wDB[0] > 0.0f && wDB[1] > 0.0f && wABD[0] <= 0.0f && wBCD[1] <= 0.0f)
  {
    float w = 1.0f / wDB[2];
    count_ = 2;
    vertices_[0] = D;
    vertices_[0].weight_ = w * wDB[0];
    vertices_[1] = B;
    vertices_[1].weight_ = w * wDB[1];
    return;
  }

  // Face Region Test

  // Weights of the tetrahedron
  float wABCD[5];

  ComputeBarycentricWeights(wABCD, A.minkowskiPoint_, B.minkowskiPoint_, C.minkowskiPoint_, D.minkowskiPoint_, origin);

  // Closest point lies on face ACB, origin point in region ABC
  if (wACB[0] > 0.0f && wACB[1] > 0.0f && wACB[2] > 0.0f && wABCD[3] <= 0.0f)
  {
    float w = 1.0f / wACB[3];
    count_ = 3;
    vertices_[0] = A;
    vertices_[0].weight_ = w * wACB[0];
    vertices_[1] = C;
    vertices_[1].weight_ = w * wACB[1];
    vertices_[2] = B;
    vertices_[2].weight_ = w * wACB[2];
    return;
  }

  // Closest point lies on face ABD, origin point in region ADB
  if (wABD[0] > 0.0f && wABD[1] > 0.0f && wABD[2] > 0.0f && wABCD[2] <= 0.0f)
  {
    float w = 1.0f / wABD[3];
    count_ = 3;
    vertices_[0] = A;
    vertices_[0].weight_ = w * wABD[0];
    vertices_[1] = B;
    vertices_[1].weight_ = w * wABD[1];
    vertices_[2] = D;
    vertices_[2].weight_ = w * wABD[2];
    return;
  }

  // Closest point lies on face ADC, origin point in region ACD
  if (wADC[0] > 0.0f && wADC[1] > 0.0f && wADC[2] > 0.0f && wABCD[1] <= 0.0f)
  {
    float w = 1.0f / wADC[3];
    count_ = 3;
    vertices_[0] = A;
    vertices_[0].weight_ = w * wADC[0];
    vertices_[1] = D;
    vertices_[1].weight_ = w * wADC[1];
    vertices_[2] = C;
    vertices_[2].weight_ = w * wADC[2];
    return;
  }

  // Closest point lies on face BDC, origin point in region BDC
  if (wBCD[0] > 0.0f && wBCD[1] > 0.0f && wBCD[2] > 0.0f && wABCD[0] <= 0.0f)
  {
    float w = 1.0f / wBCD[3];
    count_ = 3;
    vertices_[0] = B;
    vertices_[0].weight_ = w * wBCD[0];
    vertices_[1] = C;
    vertices_[1].weight_ = w * wBCD[1];
    vertices_[2] = D;
    vertices_[2].weight_ = w * wBCD[2];
    return;
  }

  if (wABCD[4] <= 0.0f)
    return;

  // Closest point lies in tetrahedron, origin point in region ABCD
  float w = 1.0f / wABCD[4];
  count_ = 4;
  vertices_[0].weight_ = w * wABCD[0];
  vertices_[1].weight_ = w * wABCD[1];
  vertices_[2].weight_ = w * wABCD[2];
  vertices_[3].weight_ = w * wABCD[3];
}

void GJK::Simplex::GetClosestPoints(Vec3& outA, Vec3& outB)
{
  // Using convex combination, we can apply the coefficients we got from barycentric formulas during our simplex solver to compute the closest points
  switch (count_)
  {
  case 1:
    outA = vertices_[0].pointA_;
    outB = vertices_[0].pointB_;
    break;
  case 2:
    outA = vertices_[0].weight_ * vertices_[0].pointA_ + vertices_[1].weight_ * vertices_[1].pointA_;
    outB = vertices_[0].weight_ * vertices_[0].pointB_ + vertices_[1].weight_ * vertices_[1].pointB_;
    break;
  case 3:
    outA = vertices_[0].weight_ * vertices_[0].pointA_ + vertices_[1].weight_ * vertices_[1].pointA_ + vertices_[2].weight_ * vertices_[2].pointA_;
    outB = vertices_[0].weight_ * vertices_[0].pointB_ + vertices_[1].weight_ * vertices_[1].pointB_ + vertices_[2].weight_ * vertices_[2].pointB_;
    break;
  case 4:
    outA = vertices_[0].weight_ * vertices_[0].pointA_ + vertices_[1].weight_ * vertices_[1].pointA_ + vertices_[2].weight_ * vertices_[2].pointA_ + vertices_[3].weight_ * vertices_[3].pointA_;
    outB = outA;
    break;
  default:
    assert(false);
    break;
  }
}

Vec3 GJK::Simplex::GetClosestPoint()
{
  // Using convex combination, we can apply the coefficients we got from barycentric formulas during our simplex solver to compute the closest point
  Vec3 result{};
  switch (count_)
  {
  case 1:
    result = vertices_[0].minkowskiPoint_;
    break;
  case 2:
    result = vertices_[0].weight_ * vertices_[0].minkowskiPoint_ + vertices_[1].weight_ * vertices_[1].minkowskiPoint_;
    break;
  case 3:
    result = vertices_[0].weight_ * vertices_[0].minkowskiPoint_ + vertices_[1].weight_ * vertices_[1].minkowskiPoint_ + vertices_[2].weight_ * vertices_[2].minkowskiPoint_;
    break;
  case 4:
    break;
  default:
    assert(false);
    break;
  }

  return result;
}

Vec3 GJK::Simplex::GetSearchDirection()
{
  // Get the general direction of the origin in between the given simplex vertices
  const Vec3 O{ 0.0f, 0.0f, 0.0f };
  Vec3 result;

  switch (count_)
  {
  case 1:
  {
    // Only has 1 vertex, general direction is a vector from point towards origin
    result = O - vertices_[0].minkowskiPoint_;
    return result;
  }
  case 2:
  {
    // Has 2 vertex, general direction is cross product ABxAO and AB
    Vec3 A = vertices_[0].minkowskiPoint_;
    Vec3 B = vertices_[1].minkowskiPoint_;
    Vec3 AB = B - A;
    Vec3 AO = O - A;

    Vec3 ABxAO = Cross(AB, AO);
    result = Cross(ABxAO, AB);
    return result;
  }
  case 3:
  {
    // Has 3 vertex, general direction is dot product ABxAC and AO, negate if sign shows other direction
    Vec3 A = vertices_[0].minkowskiPoint_;
    Vec3 B = vertices_[1].minkowskiPoint_;
    Vec3 C = vertices_[2].minkowskiPoint_;

    Vec3 AB = B - A;
    Vec3 AC = C - A;
    Vec3 AO = O - A;

    Vec3 ABxAC = Cross(AB, AC);
    float sign = Dot(ABxAC, AO);
    result = sign > 0.0f ? ABxAC : ABxAC.Negate();
    return result;
  }
  default:
    assert(false);
    break;
  }

  return result;
}

GJK::GJKOutput GJK::GJK(BoxCollider* colliderA, BoxCollider* colliderB)
{
  // Create a simplex with our colliders to start our GJK
  Simplex s;
  s.Initialize(colliderA, colliderB);

  Transform xfA = colliderA->GetRigidBody()->GetTransform();
  Transform xfB = colliderB->GetRigidBody()->GetTransform();

  // Retrieve simplex vertices as an array
  SimplexVertex* vertices = s.vertices_;

  // Stores vertices from previous simplex during loop iteration
  // Allow us to check for duplicates to prevent cycling
  uint32_t saveA[4], saveB[4];
  uint32_t saveCount = 0;

  // Setup gjk iteration values
  const int gjk_iteration_limit = 20; // Limit iteration to prevent cycling
  int       gjk_iteration = 0;

  // Start gjk iteration loop
  while (gjk_iteration < gjk_iteration_limit)
  {
    // Save our simplex to check for duplicates
    saveCount = s.count_;
    for (uint32_t i = 0; i < saveCount; ++i)
    {
      saveA[i] = vertices[i].indexA_;
      saveB[i] = vertices[i].indexB_;
    }

    // Based on the number of simplex vertices, we solve the simplex with the appropriate solver
    switch (s.count_)
    {
    case 1:
      break;
    case 2:
      s.SolveEdge();
      break;
    case 3:
      s.SolveTriangle();
      break;
    case 4:
      s.SolveTetrahedron();
      break;
    default:
      assert(false);
      break;
    }

    // GJK stop if we got 4 points, origin is in tetrahedron
    if (s.count_ == 4)
      break;

    // Get search direction.
    auto dir = s.GetSearchDirection();

    // Validate the direction
    if (Dot(dir, dir) <= FLT_EPSILON * FLT_EPSILON)
      break;

    // Generate a new vertex for the simplex
    auto vertex = vertices + s.count_;
    vertex->indexA_ = GetSupportIndex(colliderA, MultiplyT(xfA.GetRotation(), dir.Negate()));
    vertex->pointA_ = Multiply(xfA, colliderA->GetVertices()[vertex->indexA_]);
    vertex->indexB_ = GetSupportIndex(colliderB, MultiplyT(xfB.GetRotation(), dir));
    vertex->pointB_ = Multiply(xfB, colliderB->GetVertices()[vertex->indexB_]);
    vertex->minkowskiPoint_ = vertex->pointB_ - vertex->pointA_;

    ++gjk_iteration;

    // Termination criteria, we do not want to keep adding identical support points
    // See if support points created is a duplicate
    bool duplicate = false;
    for (uint32_t i = 0; i < saveCount; ++i)
    {
      if (vertex->indexA_ == saveA[i] && vertex->indexB_ == saveB[i])
      {
        duplicate = true;
        break;
      }
    }

    // Terminate the GJK as there's duplicate support point
    if (duplicate)
      break;

    // Support point is added for the next iteration
    ++s.count_;
  }

  // Set up the result for output
  GJKOutput result;
  s.GetClosestPoints(result.pointA_, result.pointB_);
  result.distance_ = Length(result.pointB_ - result.pointA_);

  return result;
}

void GJK::ComputeBarycentricWeights(float out[3], const Vec3& pointA, const Vec3& pointB, const Vec3& pointQ)
{
  // Weights are computed using edge's length
  Vec3 AB = pointB - pointA;
  Vec3 QA = pointA - pointQ;
  Vec3 QB = pointB - pointQ;

  // [0] -> u, [1] -> v, [2] -> divisor
  out[0] = Dot(QB, AB);
  out[1] = -Dot(QA, AB);
  out[2] = out[0] + out[1];
}

void GJK::ComputeBarycentricWeights(float out[4], const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointQ)
{
  // Weights are computed using triangles area
  // But weights are the ratio of the triangle area with Q over triangle area ABC
  // This can also be replaced with using cross product vector with Q projected onto cross product of AB and AC
  // This reduces the original division into multiplication case
  Vec3 AB = pointB - pointA;
  Vec3 AC = pointC - pointA;
  Vec3 ABxAC = Cross(AB, AC);

  Vec3 QA = pointA - pointQ;
  Vec3 QB = pointB - pointQ;
  Vec3 QC = pointC - pointQ;
  Vec3 QBxQC = Cross(QB, QC);
  Vec3 QCxQA = Cross(QC, QA);
  Vec3 QAxQB = Cross(QA, QB);

  // [0] - u, [1] - v, [2] - w, [3] - divisor
  out[0] = Dot(QBxQC, ABxAC);
  out[1] = Dot(QCxQA, ABxAC);
  out[2] = Dot(QAxQB, ABxAC);
  out[3] = out[0] + out[1] + out[2];
}

void GJK::ComputeBarycentricWeights(float out[5], const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD, const Vec3& pointQ)
{
  // Weights are computed using volume or tetrahedron
  // Since we have 4 points to represent tetrahedron, we can use the volume of a parallelepiped for calculation
  // By forming 3 basis vector for each combination of 3 points, we can compute its determinant ( volume of parallelepiped )
  // The sign of the determinant will ensure our result will allows us to get a positive values for all weights when computed with its divisor
  Vec3 AB = pointB - pointA;
  Vec3 AC = pointC - pointA;
  Vec3 AD = pointD - pointA;

  Vec3 QA = pointA - pointQ;
  Vec3 QB = pointB - pointQ;
  Vec3 QC = pointC - pointQ;
  Vec3 QD = pointD - pointQ;

  float volume = Det(AB, AC, AD);
  int sign = volume < 0.0f ? -1 : 1;

  out[0] = sign * Det(QB, QC, QD);
  out[1] = sign * Det(QA, QD, QC);
  out[2] = sign * Det(QA, QB, QD);
  out[3] = sign * Det(QA, QC, QB);
  out[4] = sign * volume;
}

uint32_t GJK::GetSupportIndex(BoxCollider * collider, const Vec3 & direction)
{
  const std::vector<Vec3>& vertices = collider->GetVertices();
  uint32_t count = static_cast<uint32_t>(vertices.size());

  // Search vertices for the maximal projection against given direction
  uint32_t maxIndex = 0;
  float maxProjection = Dot(direction, vertices[maxIndex]);

  for (uint32_t i = 1; i < count; ++i)
  {
    auto projection = Dot(direction, vertices[i]);
    if (projection > maxProjection)
    {
      maxIndex = i;
      maxProjection = projection;
    }
  }

  return maxIndex;
}
