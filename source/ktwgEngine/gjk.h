#pragma once

#include "vector3.h"
#include "transform.h"

/* Forward declarations */
class BoxCollider;

namespace GJK
{
  /* This struct stores the output obtain running the GJK algorithm */
  struct GJKOutput
  {
    Vec3 pointA_;    // closest point in colliderA
    Vec3 pointB_;    // closest point in colliderB
    float distance_; // Distance between the two closest points
  };

  /* Represent a vertex in the simplex */
  struct SimplexVertex
  {
    Vec3 pointA_;         // Support point from colliderA
    Vec3 pointB_;         // Support point from colliderB
    Vec3 minkowskiPoint_; // Minkowski difference of the two support points
    float weight_;        // Barycentric coefficient of the points ( Used to compute distance )
    uint32_t indexA_;     // Index of the Support point A in collider A
    uint32_t indexB_;     // Index of the Support point B in collider B
  };

  /* This struct represents a simplex which is used in GJK algorithm */
  struct Simplex
  {
    void Initialize(BoxCollider* colliderA, BoxCollider* colliderB);

    void SolveEdge();
    void SolveTriangle();
    void SolveTetrahedron();

    void GetClosestPoints(Vec3& outA, Vec3& outB);
    Vec3 GetClosestPoint();
    Vec3 GetSearchDirection();

    SimplexVertex vertices_[4]; // In 3D, the maximum simplex form is a 4-point simplex ( tetrahedron )
    uint32_t      count_;       // Current vertex count in Simplex
  };

  /* Start the GJK algorithm to compute closest points */
  GJKOutput GJK(BoxCollider* colliderA, BoxCollider* colliderB);

  /* Compute the barycentric weights of edge AB and point Q */
  void ComputeBarycentricWeights(float out[3], const Vec3& pointA, const Vec3& pointB, const Vec3& pointQ);

  /* Compute the barycentric weights of triangle ABC and point Q */
  void ComputeBarycentricWeights(float out[4], const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointQ);

  /* Compute the barycentric weights of tetrahedorn ABCD and point Q */
  void ComputeBarycentricWeights(float out[5], const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD, const Vec3& pointQ);

  /* Get support index of the support vertex of a collider with a given direction */
  uint32_t GetSupportIndex(BoxCollider* collider, const Vec3& direction);

}
