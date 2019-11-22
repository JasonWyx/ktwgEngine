/******************************************************************************/
/*!
\file       CollisionFunctions.h
\author     Koh Wen Lin
\par        Project: BPEngine
\brief      This is the interface file for CollisionFunctions.

\copyright  All content (C) 2019 DigiPen Institute of Technology Singapore. All Rights
\           Reserved.
*/
/******************************************************************************/
#pragma once

#include "vector3.h"
#include "sat.h"

class BoxCollider;
struct BPManifold;
struct ClipVertex;
struct Plane;

struct BestQuery
{
  Vec3         normal_;
  SAT::SATType type_ = SAT::eSAT_Edge;
  float        separation_ = 0.0f;
  unsigned     index_ = static_cast<unsigned>(-1);
};

struct BPSupportEdge
{
  Vec3 pointP_; // starting point of an edge
  Vec3 pointQ_; // ending point of an edge
};

//BestQuery     GetBestSeparation(const SAT::EdgeQuery& edgeQuery, const SAT::FaceQuery& faceQueryA, const SAT::FaceQuery& faceQueryB);
//BPSupportEdge GetSupportEdge(const Transform& transform, const Vec3& extents, const Vec3& normal);
//BPSupportEdge GetEdgesContact(const BPSupportEdge& supportEdge, const BPSupportEdge& supportEdgeB);
//
//void ComputeIncidentFace(const Transform& incidentTransform, const Vec3& extents, Vec3 normal, ClipVertex* out);
//void ComputeReferenceEdges(const Vec3& referenceExtents, const Transform& referenceTransform, Vec3 normal, unsigned axis, uint8_t* out, Matrix3* basis, Vec3* outExtents);
//void BuildCapsuleFaceContact(BPManifold& manifold, ICollider* colliderA, ICollider* colliderB, uint32_t faceIndex, float radius, float penetration);
//void ClosestPointBetweenEdge(Vec3& C1, Vec3& C2, const BPSupportEdge& PQ1, const BPSupportEdge& PQ2);
//Vec3 ClosestPointOnEdge(const Vec3& Q, const BPSupportEdge& PQ1);
//
//unsigned ClipCapsuleEdgeToFace(ClipVertex *out, const ClipVertex* edge, const Plane& face);
//unsigned ClipEdgeToEdge(ClipVertex* out, const ClipVertex* edgeA, const ClipVertex* edgeB);
//unsigned Clip(const Vec3& referencePos, const Vec3& extents, uint8_t* clipEdges, const Matrix3& basis, ClipVertex* incident, ClipVertex* outVertices, float* outDepths);
//unsigned Orthographic(float sign, float e, unsigned axis, unsigned clipEdge, ClipVertex* in, unsigned inCount, ClipVertex* out);
//bool     IsParallel(const BPSupportEdge& edgeA, const BPSupportEdge& edgeB);

void CheckBoxToBox(BPManifold& manifold, BoxCollider* colliderA, BoxCollider* colliderB);
