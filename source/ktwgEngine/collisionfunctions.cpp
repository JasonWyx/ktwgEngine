#include "collisionfunctions.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "contact.h"
#include <assert.h>

#define InFront( a ) \
	((a) < 0.0f)

#define Behind( a ) \
	((a) >= 0.0f)

#define On( a ) \
	((a) < 0.005f && (a) > -0.005f)

struct ClipVertex
{
  ClipVertex()
  {
    id.key = ~(uint32_t)0;
  }

  Vec3 v;
  ContactId id;
};

BestQuery GetBestSeparation(const SAT::EdgeQuery & edgeQuery, const SAT::FaceQuery & faceQueryA, const SAT::FaceQuery & faceQueryB)
{
  // Artificial axis bias to improve frame coherence
  const auto kRelTol = 0.95f;
  const auto kTol = 0.01f;
  BestQuery result;
  float faceMax = std::max(faceQueryA.maxSeparation_, faceQueryB.maxSeparation_);
  if (kRelTol * edgeQuery.maxSeparation_ > faceMax + kTol)
  {
    result.separation_ = edgeQuery.maxSeparation_;
    result.normal_ = edgeQuery.normal_;
    result.index_ = edgeQuery.index_;
    result.type_ = SAT::eSAT_Edge;
  }
  else
  {
    if (kRelTol * faceQueryB.maxSeparation_ > faceQueryA.maxSeparation_ + kTol)
    {
      result.separation_ = faceQueryB.maxSeparation_;
      result.normal_ = faceQueryB.normal_;
      result.index_ = faceQueryB.index_;
      result.type_ = SAT::eSAT_FaceB;
    }
    else
    {
      result.separation_ = faceQueryA.maxSeparation_;
      result.normal_ = faceQueryA.normal_;
      result.index_ = faceQueryA.index_;
      result.type_ = SAT::eSAT_FaceA;
    }
  }

  return result;
}

SupportEdge GetSupportEdge(const Transform & transform, const Vec3 & extents, const Vec3 & normal)
{
  SupportEdge result;

  auto n = MultiplyT(transform.GetRotationMatrix33(), normal);
  auto absN = Absolute(n);

  result.pointP_ = result.pointQ_ = Vec3{ extents.x_, extents.y_, extents.z_ };

  if (absN.x_ > absN.y_)
  {
    if (absN.y_ > absN.z_)
      result.pointQ_.z_ = -extents.z_;
    else
      result.pointQ_.y_ = -extents.y_;
  }
  else
  {
    if (absN.x_ > absN.z_)
      result.pointQ_.z_ = -extents.z_;
    else
      result.pointQ_.x_ = -extents.x_;
  }

  auto signX = Math::Sign(n.x_);
  auto signY = Math::Sign(n.y_);
  auto signZ = Math::Sign(n.z_);

  result.pointP_.x_ *= signX;
  result.pointQ_.x_ *= signX;
  result.pointP_.y_ *= signY;
  result.pointQ_.y_ *= signY;
  result.pointP_.z_ *= signZ;
  result.pointQ_.z_ *= signZ;

  result.pointP_ = Multiply(transform, result.pointP_);
  result.pointQ_ = Multiply(transform, result.pointQ_);

  return result;
}

SupportEdge GetEdgesContact(const SupportEdge& supportEdgeA, const SupportEdge& supportEdgeB)
{
  // This is used to find closest points of two line segments
  // Book reference Christer Ericson, Real Time Collision Detection
  SupportEdge contactPoints;

  auto d1 = supportEdgeA.pointQ_ - supportEdgeA.pointP_;
  auto d2 = supportEdgeB.pointQ_ - supportEdgeB.pointP_;
  auto r = supportEdgeA.pointP_ - supportEdgeB.pointP_;

  // Set up the formula
  auto a = Dot(d1, d1);
  auto b = Dot(d1, d2);
  auto c = Dot(d1, r);
  auto e = Dot(d2, d2);
  auto f = Dot(d2, r);
  auto d = a * e - b * b;

  // Get time of intersection on both edge equation
  auto s = (b * f - c * e) / d;
  auto t = (b * s + f) / d;

  contactPoints.pointP_ = supportEdgeA.pointP_ + d1 * s;
  contactPoints.pointQ_ = supportEdgeB.pointP_ + d2 * t;
  return contactPoints;
}

void ComputeIncidentFace(const Transform& incidentTransform, const Vec3& extents, Vec3 normal, ClipVertex* out)
{
  // Convert world normal into incident face's basis
  normal = MultiplyT(incidentTransform.GetRotationMatrix33(), normal);

  // Since the is incident face, incident face's normal direction point towards the reference face
  normal = normal.Negate();

  // Get the absolute value to test which face
  auto absNormal = Absolute(normal);

  // Check normal pointing direction to determine incident face to clip
  // Also set a number to index the incident face
  // x is highest, means the normal is on left or right face of the box
  if (absNormal.x_ > absNormal.y_ && absNormal.x_ > absNormal.z_)
  {
    // Right face of the box
    if (normal.x_ > 0.0f)
    {
      // Right vertices of the box
      out[0].v.Set(extents.x_, extents.y_, -extents.z_);
      out[1].v.Set(extents.x_, extents.y_, extents.z_);
      out[2].v.Set(extents.x_, -extents.y_, extents.z_);
      out[3].v.Set(extents.x_, -extents.y_, -extents.z_);

      // Assigning an index to each edges
      out[0].id.cf_.inIncident_ = 9;
      out[0].id.cf_.outIncident_ = 1;
      out[1].id.cf_.inIncident_ = 1;
      out[1].id.cf_.outIncident_ = 8;
      out[2].id.cf_.inIncident_ = 8;
      out[2].id.cf_.outIncident_ = 7;
      out[3].id.cf_.inIncident_ = 7;
      out[3].id.cf_.outIncident_ = 9;
    }
    else // Left face of the box
    {
      // Left vertices of the box
      out[0].v.Set(-extents.x_, -extents.y_, extents.z_);
      out[1].v.Set(-extents.x_, extents.y_, extents.z_);
      out[2].v.Set(-extents.x_, extents.y_, -extents.z_);
      out[3].v.Set(-extents.x_, -extents.y_, -extents.z_);

      // Assigning an index to each edges
      out[0].id.cf_.inIncident_ = 5;
      out[0].id.cf_.outIncident_ = 11;
      out[1].id.cf_.inIncident_ = 11;
      out[1].id.cf_.outIncident_ = 3;
      out[2].id.cf_.inIncident_ = 3;
      out[2].id.cf_.outIncident_ = 10;
      out[3].id.cf_.inIncident_ = 10;
      out[3].id.cf_.outIncident_ = 5;
    }
  }
  // Normal is on top or bottom face of the box
  else if (absNormal.y_ > absNormal.x_ && absNormal.y_ > absNormal.z_)
  {
    // Top face of the box
    if (normal.y_ > 0.0f)
    {
      // Top vertices of the box
      out[0].v.Set(-extents.x_, extents.y_, extents.z_);
      out[1].v.Set(extents.x_, extents.y_, extents.z_);
      out[2].v.Set(extents.x_, extents.y_, -extents.z_);
      out[3].v.Set(-extents.x_, extents.y_, -extents.z_);

      // Assigning an index to each edges
      out[0].id.cf_.inIncident_ = 3;
      out[0].id.cf_.outIncident_ = 0;
      out[1].id.cf_.inIncident_ = 0;
      out[1].id.cf_.outIncident_ = 1;
      out[2].id.cf_.inIncident_ = 1;
      out[2].id.cf_.outIncident_ = 2;
      out[3].id.cf_.inIncident_ = 2;
      out[3].id.cf_.outIncident_ = 3;
    }
    else // Bottom face of the box
    {
      // Bottom vertices of the box
      out[0].v.Set(extents.x_, -extents.y_, extents.z_);
      out[1].v.Set(-extents.x_, -extents.y_, extents.z_);
      out[2].v.Set(-extents.x_, -extents.y_, -extents.z_);
      out[3].v.Set(extents.x_, -extents.y_, -extents.z_);

      // Assigning an index to each edges
      out[0].id.cf_.inIncident_ = 7;
      out[0].id.cf_.outIncident_ = 4;
      out[1].id.cf_.inIncident_ = 4;
      out[1].id.cf_.outIncident_ = 5;
      out[2].id.cf_.inIncident_ = 5;
      out[2].id.cf_.outIncident_ = 6;
      out[3].id.cf_.inIncident_ = 5;
      out[3].id.cf_.outIncident_ = 6;
    }
  }
  // Normal is on front or back of box
  else
  {
    // Back face of the box
    if (normal.z_ > 0.0f)
    {
      // Back vertices of the box
      out[0].v.Set(-extents.x_, extents.y_, extents.z_);
      out[1].v.Set(-extents.x_, -extents.y_, extents.z_);
      out[2].v.Set(extents.x_, -extents.y_, extents.z_);
      out[3].v.Set(extents.x_, extents.y_, extents.z_);

      // Assigning an index to each edges
      out[0].id.cf_.inIncident_ = 0;
      out[0].id.cf_.outIncident_ = 11;
      out[1].id.cf_.inIncident_ = 11;
      out[1].id.cf_.outIncident_ = 4;
      out[2].id.cf_.inIncident_ = 4;
      out[2].id.cf_.outIncident_ = 8;
      out[3].id.cf_.inIncident_ = 8;
      out[3].id.cf_.outIncident_ = 0;
    }
    else // Front face of the box
    {
      // Front vertices of the box
      out[0].v.Set(extents.x_, -extents.y_, -extents.z_);
      out[1].v.Set(-extents.x_, -extents.y_, -extents.z_);
      out[2].v.Set(-extents.x_, extents.y_, -extents.z_);
      out[3].v.Set(extents.x_, extents.y_, -extents.z_);

      // Assigning an index to each edges
      out[0].id.cf_.inIncident_ = 9;
      out[0].id.cf_.outIncident_ = 6;
      out[1].id.cf_.inIncident_ = 6;
      out[1].id.cf_.outIncident_ = 10;
      out[2].id.cf_.inIncident_ = 10;
      out[2].id.cf_.outIncident_ = 2;
      out[3].id.cf_.inIncident_ = 2;
      out[3].id.cf_.outIncident_ = 9;
    }
  }

  // Convert the clip vertex from incident basis to world basis
  for (auto i = 0; i < 4; ++i)
    out[i].v = Multiply(incidentTransform, out[i].v);
}

void ComputeReferenceEdges(const Vec3& referenceExtents, const Transform & referenceTransform, Vec3 normal, unsigned axis, uint8_t* out, Matrix3* basis, Vec3* outExtents)
{
  // Convert normal from world to reference face basis
  auto rotationMat = referenceTransform.GetRotationMatrix33();
  normal = MultiplyT(rotationMat, normal);

  // Reduce 6 face axis to 3 face axis as 0, 1, 2 are A's axis, and 3, 4, 5 are B's axis
  // Behaviour of face 0, 1, 2 are similar with respect to face 3, 4, 5
  if (axis >= 3)
    axis -= 3;

  switch (axis)
  {
    // Right or Left face of the box
  case 0:
    // Right face of the box
    if (normal.x_ > 0.0f)
    {
      out[0] = 1;
      out[1] = 8;
      out[2] = 7;
      out[3] = 9;

      outExtents->Set(referenceExtents.y_, referenceExtents.z_, referenceExtents.x_);
      basis->SetRows(rotationMat.GetRow(1), rotationMat.GetRow(2), rotationMat.GetRow(0));
    }
    else // Left face of the box
    {
      out[0] = 11;
      out[1] = 3;
      out[2] = 10;
      out[3] = 5;

      outExtents->Set(referenceExtents.z_, referenceExtents.y_, referenceExtents.x_);
      basis->SetRows(rotationMat.GetRow(2), rotationMat.GetRow(1), rotationMat.GetRow(0).Negate());
    }
    break;
    // Top or bottom face of the box
  case 1:
    // Top face of the box
    if (normal.y_ > 0.0f)
    {
      out[0] = 0;
      out[1] = 1;
      out[2] = 2;
      out[3] = 3;

      outExtents->Set(referenceExtents.z_, referenceExtents.x_, referenceExtents.y_);
      basis->SetRows(rotationMat.GetRow(2), rotationMat.GetRow(0), rotationMat.GetRow(1));
    }
    else // Bottom face of the box
    {
      out[0] = 4;
      out[1] = 5;
      out[2] = 6;
      out[3] = 7;

      outExtents->Set(referenceExtents.z_, referenceExtents.x_, referenceExtents.y_);
      basis->SetRows(rotationMat.GetRow(2), rotationMat.GetRow(0).Negate(), rotationMat.GetRow(1).Negate());
    }
    break;
    // Back or front face of the box
  case 2:
    // Back face of the box
    if (normal.z_ > 0.0f)
    {
      out[0] = 11;
      out[1] = 4;
      out[2] = 8;
      out[3] = 0;

      outExtents->Set(referenceExtents.y_, referenceExtents.x_, referenceExtents.z_);
      basis->SetRows(rotationMat.GetRow(1).Negate(), rotationMat.GetRow(0), rotationMat.GetRow(2));
    }
    else // Front face of the box
    {
      out[0] = 6;
      out[1] = 10;
      out[2] = 2;
      out[3] = 9;

      outExtents->Set(referenceExtents.y_, referenceExtents.x_, referenceExtents.z_);
      basis->SetRows(rotationMat.GetRow(1).Negate(), rotationMat.GetRow(0).Negate(), rotationMat.GetRow(2).Negate());
    }
    break;
  }
}

unsigned Clip(const Vec3& referencePos, const Vec3& extents, uint8_t* clipEdges, const Matrix3& basis, ClipVertex* incident, ClipVertex* outVertices, float* outDepths)
{
  unsigned inCount = 4;
  unsigned outCount;
  ClipVertex in[8];
  ClipVertex out[8];

  // Convert to reference face's basis
  for (auto i = 0; i < 4; ++i)
    in[i].v = MultiplyT(basis, incident[i].v - referencePos);

  outCount = Orthographic(1.0f, extents.x_, 0, clipEdges[0], in, inCount, out);

  if (!outCount)
    return 0;

  inCount = Orthographic(1.0f, extents.y_, 1, clipEdges[1], out, outCount, in);

  if (!inCount)
    return 0;

  outCount = Orthographic(-1.0f, extents.x_, 0, clipEdges[2], in, inCount, out);

  if (!outCount)
    return 0;

  inCount = Orthographic(-1.0f, extents.y_, 1, clipEdges[3], out, outCount, in);

  // Keep incident vertices behind the reference face
  outCount = 0;
  for (auto i = 0U; i < inCount; ++i)
  {
    float d = in[i].v.z_ - extents.z_;

    if (d <= 0.0f)
    {
      outVertices[outCount].v = Multiply(basis, in[i].v) + referencePos;
      outVertices[outCount].id = in[i].id;
      outDepths[outCount++] = d;
    }
  }

  assert(outCount <= 8);

  return outCount;
}

unsigned Orthographic(float sign, float e, unsigned axis, unsigned clipEdge, ClipVertex* in, unsigned inCount, ClipVertex* out)
{
  unsigned outCount = 0;
  ClipVertex a = in[inCount - 1];

  for (auto i = 0U; i < inCount; ++i)
  {
    ClipVertex b = in[i];

    float da = sign * a.v[axis] - e;
    float db = sign * b.v[axis] - e;

    ClipVertex cv;

    // B
    if (((InFront(da) && InFront(db)) || On(da) || On(db)))
    {
      assert(outCount < 8);
      out[outCount++] = b;
    }

    // I
    else if (InFront(da) && Behind(db))
    {
      cv.id.cf_ = b.id.cf_;
      cv.v = a.v + (b.v - a.v) * (da / (da - db));
      cv.id.cf_.outReference_ = static_cast<uint8_t>(clipEdge);
      cv.id.cf_.outIncident_ = 0;
      assert(outCount < 8);
      out[outCount++] = cv;
    }

    // I, B
    else if (Behind(da) && InFront(db))
    {
      cv.id.cf_ = a.id.cf_;
      cv.v = a.v + (b.v - a.v) * (da / (da - db));
      cv.id.cf_.inReference_ = static_cast<uint8_t>(clipEdge);
      cv.id.cf_.inIncident_ = 0;
      assert(outCount < 8);
      out[outCount++] = cv;

      assert(outCount < 8);
      out[outCount++] = b;
    }

    a = b;
  }

  return outCount;
}

bool IsParallel(const SupportEdge& edgeA, const SupportEdge& edgeB)
{
  auto d1 = edgeA.pointQ_ - edgeA.pointP_;
  auto dLen1 = Length(d1);
  if (dLen1 < LINEAR_SLOP)
    return false;

  auto d2 = edgeB.pointQ_ - edgeB.pointP_;
  auto dLen2 = Length(d2);
  if (dLen2 < LINEAR_SLOP)
    return false;

  auto kTol = 0.005f;
  auto n = Cross(d1, d2);
  return Length(n) < kTol * dLen1 * dLen2;
}

void CheckBoxToBox(Manifold& manifold, BoxCollider* colliderA, BoxCollider* colliderB)
{
  RigidBody* bodyA = colliderA->GetRigidBody();
  RigidBody* bodyB = colliderB->GetRigidBody();

  // Get body transform ( World transforms )
  Transform transformA = bodyA->GetTransform();
  Transform transformB = bodyB->GetTransform();

  // Get AABB's world extents
  Vec3 extentA = colliderA->GetExtents();
  Vec3 extentB = colliderB->GetExtents();
  Vec3 worldScaleA = transformA.GetScale();
  Vec3 worldScaleB = transformB.GetScale();

  // Get box local transform to body ( Local Transforms ), need to shift transform using offset by extent for world position
  Vec3 offsetA = colliderA->GetLocal().GetPosition();
  Vec3 offsetB = colliderB->GetLocal().GetPosition();

  // Offset is relative to rigidbody center
  offsetA = offsetA * worldScaleA;
  offsetB = offsetB * worldScaleB;

  Transform xfA, xfB;
  xfA.SetPosition(offsetA);
  xfB.SetPosition(offsetB);

  extentA = Multiply(worldScaleA, extentA);
  extentB = Multiply(worldScaleB, extentB);

  // Compute both box's world transformations
  transformA = MultiplyNS(transformA, xfA);
  transformB = MultiplyNS(transformB, xfB);

  // Get rotational transform to convert B basis to A basis
  Matrix3 rotMatA = transformA.GetRotationMatrix33();
  Matrix3 rotMatB = transformB.GetRotationMatrix33();
  Matrix3 rotMat = rotMatB * rotMatA.Transpose();

  // Build the absolute value for the matrix as due to symmetry, we can use it for comparison
  Matrix3 absRotMat;
  bool parallel = false;
  const float kCosTol = 1.0e-3f;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      float val = abs(rotMat.m_[i][j]);
      absRotMat.m_[i][j] = val;

      if (val + kCosTol >= 1.0f)
        parallel = true;
    }
  }

  SAT::FaceQuery faceQueryA;
  SAT::FaceQuery faceQueryB;

  faceQueryA.maxSeparation_ = -FLT_MAX;
  faceQueryB.maxSeparation_ = -FLT_MAX;

  // There are 6 face axis to check between two Boxes
  if (!SAT::CheckFaceSeparation(faceQueryA, faceQueryB, transformA, transformB, extentA, extentB, rotMatA, rotMatB, rotMat, absRotMat))
    return;

  SAT::EdgeQuery edgeQuery;
  edgeQuery.maxSeparation_ = -FLT_MAX;

  if (!parallel)
  {
    //if(!SAT::CheckEdgeSeparation(edgeQuery, transformA, transformB, extentA, extentB, rotMatA, rotMatB, rotMat, absRotMat))
    //   return;
  }

  BestQuery bestQuery = GetBestSeparation(edgeQuery, faceQueryA, faceQueryB);

  // If B position is left of A, need to negate the normal
  if (Dot(bestQuery.normal_, transformB.GetPosition() - transformA.GetPosition()) < 0.0f)
    bestQuery.normal_ = bestQuery.normal_.Negate();

  manifold.useOffset_ = true;

  // If SAT shows that the furthest penetration is an edge
  if (bestQuery.type_ == SAT::eSAT_Edge)
  {
    // Rotate world normal to A's space
    bestQuery.normal_ = Multiply(rotMatA, bestQuery.normal_);

    if (Dot(bestQuery.normal_, transformB.GetPosition() - transformA.GetPosition()) < 0.0f)
      bestQuery.normal_ = bestQuery.normal_.Negate();

    SupportEdge supportEdgeA = GetSupportEdge(transformA, extentA, bestQuery.normal_);
    SupportEdge supportEdgeB = GetSupportEdge(transformB, extentB, bestQuery.normal_.Negate());

    SupportEdge contactPoints = GetEdgesContact(supportEdgeA, supportEdgeB);

    manifold.normal_ = bestQuery.normal_;
    manifold.contactCount_ = 1;

    ContactId id;
    ContactInfo* contactInfo = manifold.contactInfos_;
    id.key = bestQuery.index_;
    contactInfo->id_ = id;
    contactInfo->penetration_ = bestQuery.separation_;
    contactInfo->position_ = (contactPoints.pointP_ + contactPoints.pointQ_) * 0.5f;
    contactInfo->localPointA_ = MultiplyT(transformA, contactPoints.pointP_);
    contactInfo->offsetA_ = offsetA;
    contactInfo->localPointB_ = MultiplyT(transformB, contactPoints.pointQ_);
    contactInfo->offsetB_ = offsetB;
  }
  else
  {
    Transform referenceFaceTransform = transformA;
    Transform incidentFaceTransform = transformB;
    Vec3 referenceExtents = extentA;
    Vec3 incidentExtents = extentB;
    bool flip = false;

    if (bestQuery.type_ == SAT::eSAT_FaceB)
    {
      referenceFaceTransform = transformB;
      incidentFaceTransform = transformA;
      referenceExtents = extentB;
      incidentExtents = extentA;
      bestQuery.normal_ = bestQuery.normal_.Negate();
      flip = true;
    }

    // Generate incident clip vertices
    // Maximum of 4 incident vertices in 3D face
    ClipVertex incident[4];
    ComputeIncidentFace(incidentFaceTransform, incidentExtents, bestQuery.normal_, incident);

    // Generate reference clip edges
    // Maximum of 4 clip edges in 3D face
    uint8_t clipEdges[4];
    Matrix3 basis;
    Vec3 extents;
    ComputeReferenceEdges(referenceExtents, referenceFaceTransform, bestQuery.normal_, bestQuery.index_, clipEdges, &basis, &extents);

    // Clip the incident face against the reference face planes
    ClipVertex result[8];
    float depths[8];
    int8_t outNum;
    outNum = (int8_t)Clip(referenceFaceTransform.GetPosition(), extents, clipEdges, basis, incident, result, depths);

    if (outNum)
    {
      manifold.contactCount_ = outNum;
      manifold.normal_ = flip ? bestQuery.normal_.Negate() : bestQuery.normal_;

      for (int8_t i = 0; i < outNum; ++i)
      {
        ContactInfo* contactInfo = manifold.contactInfos_ + i;

        ContactId id = result[i].id;

        referenceFaceTransform.SetScale(worldScaleA);
        incidentFaceTransform.SetScale(worldScaleB);
        contactInfo->localPointA_ = MultiplyT(referenceFaceTransform, result[i].v + abs(depths[i]) * manifold.normal_);
        contactInfo->offsetA_ = offsetA;
        contactInfo->localPointB_ = MultiplyT(incidentFaceTransform, result[i].v);
        contactInfo->offsetB_ = offsetB;

        if (flip)
        {
          incidentFaceTransform.SetScale(worldScaleA);
          referenceFaceTransform.SetScale(worldScaleB);
          std::swap(id.cf_.inIncident_, id.cf_.inReference_);
          std::swap(id.cf_.outIncident_, id.cf_.outReference_);
          contactInfo->localPointA_ = MultiplyT(incidentFaceTransform, result[i].v + abs(depths[i]) * manifold.normal_);
          contactInfo->offsetA_ = offsetB;
          contactInfo->localPointB_ = MultiplyT(referenceFaceTransform, result[i].v);
          contactInfo->offsetB_ = offsetA;
        }

        contactInfo->id_ = result[i].id;
        contactInfo->position_ = result[i].v;  // incident face vertices
        contactInfo->penetration_ = depths[i];
      }
    }
  }
}
