#include "collisionfunctions.h"
#include "boxcollider.h"
#include "rigidbody.h"

void CheckBoxToBox(BPManifold& manifold, BoxCollider* colliderA, BoxCollider* colliderB)
{
  RigidBody* bodyA = colliderA->GetRigidBody();
  RigidBody* bodyB = colliderB->GetRigidBody();

  // Get body transform ( World transforms )
  auto transformA = bodyA->GetTransform();
  auto transformB = bodyB->GetTransform();

  // Get AABB's world extents
  auto extentA = boxA->GetExtents();
  auto extentB = boxB->GetExtents();
  auto worldScaleA = transformA.GetScale();
  auto worldScaleB = transformB.GetScale();

  // Get box local transform to body ( Local Transforms ), need to shift transform using offset by extent for world position
  auto offsetA = boxA->GetLocal().GetPosition();
  auto offsetB = boxB->GetLocal().GetPosition();

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
  auto rotMatA = transformA.GetRotationMatrix33();
  auto rotMatB = transformB.GetRotationMatrix33();
  auto rotMat = rotMatB * rotMatA.Transpose();

  // Build the absolute value for the matrix as due to symmetry, we can use it for comparison
  Matrix3 absRotMat;
  auto parallel = false;
  const auto kCosTol = 1.0e-3f;
  for (auto i = 0; i < 3; ++i)
  {
    for (auto j = 0; j < 3; ++j)
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

  auto bestQuery = GetBestSeparation(edgeQuery, faceQueryA, faceQueryB);

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

    auto supportEdgeA = GetSupportEdge(transformA, extentA, bestQuery.normal_);
    auto supportEdgeB = GetSupportEdge(transformB, extentB, bestQuery.normal_.Negate());

    auto contactPoints = GetEdgesContact(supportEdgeA, supportEdgeB);

    manifold.normal_ = bestQuery.normal_;
    manifold.contactCount_ = 1;

    BPContactId id;
    auto contactInfo = manifold.contactInfos_;
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
    auto referenceFaceTransform = transformA;
    auto incidentFaceTransform = transformB;
    auto referenceExtents = extentA;
    auto incidentExtents = extentB;
    auto flip = false;

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

      for (auto i = (int8_t)0; i < outNum; ++i)
      {
        auto contactInfo = manifold.contactInfos_ + i;

        BPContactId id = result[i].id;

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
