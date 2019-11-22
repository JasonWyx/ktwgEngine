#pragma once

#include "vector3.h"
#include "sat.h"

class BoxCollider;
struct Manifold;
struct ClipVertex;
struct Plane;

struct BestQuery
{
  Vec3         normal_;
  SAT::SATType type_ = SAT::eSAT_Edge;
  float        separation_ = 0.0f;
  unsigned     index_ = static_cast<unsigned>(-1);
};

struct SupportEdge
{
  Vec3 pointP_; // starting point of an edge
  Vec3 pointQ_; // ending point of an edge
};

BestQuery   GetBestSeparation(const SAT::EdgeQuery& edgeQuery, const SAT::FaceQuery& faceQueryA, const SAT::FaceQuery& faceQueryB);
SupportEdge GetSupportEdge(const Transform& transform, const Vec3& extents, const Vec3& normal);
SupportEdge GetEdgesContact(const SupportEdge& supportEdgeA, const SupportEdge& supportEdgeB);

void ComputeIncidentFace(const Transform& incidentTransform, const Vec3& extents, Vec3 normal, ClipVertex* out);
void ComputeReferenceEdges(const Vec3& referenceExtents, const Transform& referenceTransform, Vec3 normal, unsigned axis, uint8_t* out, Matrix3* basis, Vec3* outExtents);

unsigned Clip(const Vec3& referencePos, const Vec3& extents, uint8_t* clipEdges, const Matrix3& basis, ClipVertex* incident, ClipVertex* outVertices, float* outDepths);
unsigned Orthographic(float sign, float e, unsigned axis, unsigned clipEdge, ClipVertex* in, unsigned inCount, ClipVertex* out);
bool     IsParallel(const SupportEdge& edgeA, const SupportEdge& edgeB);

void CheckBoxToBox(Manifold& manifold, BoxCollider* colliderA, BoxCollider* colliderB);
