#include "contactsolver.h"
#include "contact.h"
#include "boxcollider.h"
#include "rigidbody.h"
#include "island.h"

void ContactInfoState::Set(ContactInfo* contactInfo, ContactState* contactState)
{
  relativePosA_ = contactInfo->position_ - contactState->centerA_;
  relativePosB_ = contactInfo->position_ - contactState->centerB_;
  position_ = contactInfo->position_;
  localPointA_ = contactInfo->localPointA_;
  localPointB_ = contactInfo->localPointB_;
  offsetA_ = contactInfo->offsetA_;
  offsetB_ = contactInfo->offsetB_;
  penetration_ = contactInfo->penetration_;
  normalImpulse_ = contactInfo->normalImpulse_;
  tangentImpulse_[0] = contactInfo->tangentImpulse_[0];
  tangentImpulse_[1] = contactInfo->tangentImpulse_[1];
}

void ContactState::Set(Contact* contact)
{
  auto bodyA = contact->pair_.colliders_[0]->GetRigidBody();
  auto bodyB = contact->pair_.colliders_[1]->GetRigidBody();

  centerA_ = bodyA->m_Transform.GetPosition();
  centerB_ = bodyB->m_Transform.GetPosition();
  localInertiaA_ = bodyA->m_InvInertia;
  localInertiaB_ = bodyB->m_InvInertia;
  localCenterA_ = bodyA->m_Sweep.localCenter_;
  localCenterB_ = bodyB->m_Sweep.localCenter_;
  scalarA_ = bodyA->GetTransform().GetScale();
  scalarB_ = bodyB->GetTransform().GetScale();
  inertiaA_ = bodyA->m_WorldInvInertia;
  inertiaB_ = bodyB->m_WorldInvInertia;
  invMassA_ = bodyA->m_InvMass;
  invMassB_ = bodyB->m_InvMass;
  indexA_ = bodyA->m_IslandId;
  indexB_ = bodyB->m_IslandId;

  auto maxScaleA = Math::Max(bodyA->GetTransform().GetScale().x_, Math::Max(bodyA->GetTransform().GetScale().y_, bodyA->GetTransform().GetScale().z_));
  auto maxScaleB = Math::Max(bodyB->GetTransform().GetScale().x_, Math::Max(bodyB->GetTransform().GetScale().y_, bodyB->GetTransform().GetScale().z_));

  radiusA_ = maxScaleA * contact->pair_.colliders_[0]->GetRadius();
  radiusB_ = maxScaleB * contact->pair_.colliders_[1]->GetRadius();
  friction_ = contact->friction_;
  restitution_ = contact->restitution_;
  useOffset_ = contact->manifold_.useOffset_;
  normal_ = contact->manifold_.normal_;
  tangentVectors_[0] = contact->manifold_.tangentVectors_[0];
  tangentVectors_[1] = contact->manifold_.tangentVectors_[1];
  contactCount_ = contact->manifold_.contactCount_;

  for (auto j = (int8_t)0; j < contactCount_; ++j)
  {
    auto contactInfoState = contactInfoStates_ + j;
    auto contactInfo = contact->manifold_.contactInfos_ + j;
    contactInfoState->Set(contactInfo, this);
  }
}

ContactSolver::ContactSolver(Island* island)
  : contactCount_{ island->contactCount_ }, contactCapacity_{ island->contactCapacity_ }
{
  contactStates_ = new ContactState [contactCapacity_];

  // Set up out solver with velocities informations
  linearVelocities_ = island->linearVelocities_;
  angularVelocities_ = island->angularVelocities_;
  island_ = island;

  positions_ = island->positions_;
  orientations_ = island->orientations_;
  inverseInertias_ = island->inverseInertias_;

  for (auto i = 0U; i < contactCount_; ++i)
  {
    auto contact = island_->contacts_[i];
    auto contactState = contactStates_ + i;
    contactState->Set(contact);
  }
}

ContactSolver::~ContactSolver()
{
  ShutDown();
}

void ContactSolver::ShutDown()
{
  delete[] contactStates_;
}

void ContactSolver::StoreImpulses()
{
  // Update the contact's old impulses with our new ones
  for (auto i = 0U; i < contactCount_; ++i)
  {
    auto contactState = contactStates_ + i;
    auto contact = island_->contacts_[i];

    for (auto j = (int8_t)0; j < contactState->contactCount_; ++j)
    {
      auto oldContactInfo = contact->manifold_.contactInfos_ + j;
      auto contactInfoState = contactState->contactInfoStates_ + j;
      oldContactInfo->normalImpulse_ = contactInfoState->normalImpulse_;
      oldContactInfo->tangentImpulse_[0] = contactInfoState->tangentImpulse_[0];
      oldContactInfo->tangentImpulse_[1] = contactInfoState->tangentImpulse_[1];
    }
  }
}

void ContactSolver::WarmStart(float dt)
{
  for (auto i = 0U; i < contactCount_; ++i)
  {
    auto contactState = contactStates_ + i;

    auto linearVelocityA = linearVelocities_[contactState->indexA_];
    auto angularVelocityA = angularVelocities_[contactState->indexA_];
    auto linearVelocityB = linearVelocities_[contactState->indexB_];
    auto angularVelocityB = angularVelocities_[contactState->indexB_];

    for (auto j = (int8_t)0; j < contactState->contactCount_; ++j)
    {
      auto contactInfoState = contactState->contactInfoStates_ + j;

      // Precalculate JM^-1JT for contact and friction constraints
      auto raCn = Cross(contactInfoState->relativePosA_, contactState->normal_);
      auto rbCn = Cross(contactInfoState->relativePosB_, contactState->normal_);
      auto normalMass = contactState->invMassA_ + contactState->invMassB_;

      float tangentMass[2];
      tangentMass[0] = normalMass;
      tangentMass[1] = normalMass;

      normalMass += Dot(raCn, Multiply(contactState->inertiaA_, raCn)) + Dot(rbCn, Multiply(contactState->inertiaB_, rbCn));
      contactInfoState->normalMass_ = normalMass != 0.0f ? 1.0f / normalMass : 0.0f;

      for (auto k = 0; k < 2; ++k)
      {
        auto raCt = Cross(contactState->tangentVectors_[k], contactInfoState->relativePosA_);
        auto rbCt = Cross(contactState->tangentVectors_[k], contactInfoState->relativePosB_);
        tangentMass[k] += Dot(raCt, Multiply(contactState->inertiaA_, raCt)) + Dot(rbCt, Multiply(contactState->inertiaB_, rbCt));
        contactInfoState->tangentMass_[k] = tangentMass[k] != 0.0f ? 1.0f / tangentMass[k] : 0.0f;
      }

      // Precalculate bias factor
      contactInfoState->bias_ = -BAUMGARTE * (1.0f / dt) * Math::Min(0.0f, contactInfoState->penetration_ + PENETRATION_SLOP);

      // Warm start contact
      auto P = contactState->normal_ * contactInfoState->normalImpulse_;

      P += contactState->tangentVectors_[0] * contactInfoState->tangentImpulse_[0];
      P += contactState->tangentVectors_[1] * contactInfoState->tangentImpulse_[1];

      linearVelocityA -= P * contactState->invMassA_;
      angularVelocityA -= Multiply(contactState->inertiaA_, Cross(contactInfoState->relativePosA_, P));

      linearVelocityB += P * contactState->invMassB_;
      angularVelocityB += Multiply(contactState->inertiaB_, Cross(contactInfoState->relativePosB_, P));

      // Add in restitution bias
      auto angularA = Cross(angularVelocityA, contactInfoState->relativePosA_);
      auto angularB = Cross(angularVelocityB, contactInfoState->relativePosB_);
      auto dv = Dot((linearVelocityB + angularB) - (linearVelocityA + angularA), contactState->normal_);

      if (dv < -1.0f)
        contactInfoState->bias_ += -(contactState->restitution_) * dv;
    }

    linearVelocities_[contactState->indexA_] = linearVelocityA;
    angularVelocities_[contactState->indexA_] = angularVelocityA;
    linearVelocities_[contactState->indexB_] = linearVelocityB;
    angularVelocities_[contactState->indexB_] = angularVelocityB;
  }
}

void ContactSolver::SolveVelocityConstraints()
{
  for (auto i = 0U; i < contactCount_; ++i)
  {
    auto contactState = contactStates_ + i;

    // Get all velocities informations from contact
    auto linearVelocityA = linearVelocities_[contactState->indexA_];
    auto angularVelocityA = angularVelocities_[contactState->indexA_];
    auto invMassA = contactState->invMassA_;
    auto inertiaA = contactState->inertiaA_;

    auto linearVelocityB = linearVelocities_[contactState->indexB_];
    auto angularVelocityB = angularVelocities_[contactState->indexB_];
    auto invMassB = contactState->invMassB_;
    auto inertiaB = contactState->inertiaB_;

    auto tangentVectors = contactState->tangentVectors_;
    auto friction = contactState->friction_;
    auto normal = contactState->normal_;

    auto contactCount = contactState->contactCount_;
    auto contactInfoStates = contactState->contactInfoStates_;

    for (auto j = (int8_t)0; j < contactCount; ++j)
    {
      auto contactInfoState = contactInfoStates + j;

      // Compute relative velocity from contact A
      auto angularA = Cross(angularVelocityA, contactInfoState->relativePosA_);
      auto angularB = Cross(angularVelocityB, contactInfoState->relativePosB_);
      auto dv = (linearVelocityB + angularB) - (linearVelocityA + angularA);

      for (auto k = 0U; k < 2; ++k)
      {
        // impulse = mass * velocity vector
        auto impulse = -Dot(dv, tangentVectors[k]) * contactInfoState->tangentMass_[k];

        // Compute frictional impulse
        auto maxImpulse = friction * contactInfoState->normalImpulse_;

        // Clamp frictional impulse
        auto oldImpulse = contactInfoState->tangentImpulse_[k];
        auto totalImpulse = oldImpulse + impulse;

        contactInfoState->tangentImpulse_[k] = Math::Clamp(-maxImpulse, maxImpulse, totalImpulse);

        impulse = contactInfoState->tangentImpulse_[k] - oldImpulse;

        // Apply friction impulse
        auto resultantImpulse = tangentVectors[k] * impulse;
        linearVelocityA -= resultantImpulse * invMassA;
        angularVelocityA -= Multiply(inertiaA, Cross(contactInfoState->relativePosA_, resultantImpulse));

        linearVelocityB += resultantImpulse * invMassB;
        angularVelocityB += Multiply(inertiaB, Cross(contactInfoState->relativePosB_, resultantImpulse));
      }

      // Normal
      angularA = Cross(angularVelocityA, contactInfoState->relativePosA_);
      angularB = Cross(angularVelocityB, contactInfoState->relativePosB_);
      dv = (linearVelocityB + angularB) - (linearVelocityA + angularA);

      // Normal impulse
      auto vn = Dot(dv, normal);

      // Factor in bias to calculate impulse scalar
      auto impulse = contactInfoState->normalMass_ * (-vn + contactInfoState->bias_);

      // Clamp impulse
      auto oldImpulse = contactInfoState->normalImpulse_;
      contactInfoState->normalImpulse_ = Math::Max(contactInfoState->normalImpulse_ + impulse, 0.0f);
      impulse = contactInfoState->normalImpulse_ - oldImpulse;

      // Apply impulse
      auto resultantImpulse = impulse * normal;

      linearVelocityA -= resultantImpulse * invMassA;
      angularVelocityA -= Multiply(inertiaA, Cross(contactInfoState->relativePosA_, resultantImpulse));

      linearVelocityB += resultantImpulse * invMassB;
      angularVelocityB += Multiply(inertiaB, Cross(contactInfoState->relativePosB_, resultantImpulse));
    }

    linearVelocities_[contactState->indexA_] = linearVelocityA;
    angularVelocities_[contactState->indexA_] = angularVelocityA;
    linearVelocities_[contactState->indexB_] = linearVelocityB;
    angularVelocities_[contactState->indexB_] = angularVelocityB;
  }
}

bool ContactSolver::SolvePositionConstraints()
{
  auto minSeparation = 0.0f;

  for (auto i = 0U; i < contactCount_; ++i)
  {
    auto contactState = contactStates_ + i;
    auto positionA = positions_[contactState->indexA_];
    auto orientationA = orientations_[contactState->indexA_];
    auto inertiaA = inverseInertias_[contactState->indexA_];
    auto localCenterA = contactState->localCenterA_;
    auto scalarA = contactState->scalarA_;
    auto localInertiaA = contactState->localInertiaA_;
    auto radiusA = contactState->radiusA_;

    auto positionB = positions_[contactState->indexB_];
    auto orientationB = orientations_[contactState->indexB_];
    auto inertiaB = inverseInertias_[contactState->indexB_];
    auto localCenterB = contactState->localCenterB_;
    auto scalarB = contactState->scalarB_;
    auto localInertiaB = contactState->localInertiaB_;
    auto radiusB = contactState->radiusB_;

    auto invMassA = contactState->invMassA_;
    auto invMassB = contactState->invMassB_;
    auto normal = contactState->normal_;

    auto contactCount = contactState->contactCount_;
    auto contactInfoStates = contactState->contactInfoStates_;
    auto useOffset = contactState->useOffset_;

    for (auto j = 0; j < contactCount; ++j)
    {
      auto contactInfoState = contactInfoStates + j;

      Transform transformA, transformB;
      auto offsetA = contactInfoState->offsetA_;
      transformA.SetRotation(orientationA);
      transformA.SetPosition(positionA - MultiplyT(transformA.GetRotationMatrix33(), localCenterA));
      transformA.SetScale(scalarA);

      auto offsetB = contactInfoState->offsetB_;
      transformB.SetRotation(orientationB);
      transformB.SetPosition(positionB - Multiply(transformB.GetRotationMatrix33(), localCenterB));
      transformB.SetScale(scalarB);

      if (useOffset)
      {
        transformA.SetPosition(transformA.GetPosition() + offsetA);
        transformB.SetPosition(transformB.GetPosition() + offsetB);
      }

      auto c1 = Multiply(transformA, contactInfoState->localPointA_);
      auto c2 = Multiply(transformB, contactInfoState->localPointB_);
      auto point = c2;
      auto ds = Dot(c2 - c1, normal);
      auto sign = ds > 0.0f ? 1 : -1;
      auto separation = sign * ds - radiusA - radiusB;

      minSeparation = Math::Min(minSeparation, separation);

      float C = Math::Clamp(-MAX_LINEAR_CORRECTION, 0.0f, BAUMGARTE * (separation + LINEAR_SLOP));

      auto relativePosA = point - positionA;
      auto relativePosB = point - positionB;

      auto relativeOrientationA = Cross(relativePosA, normal);
      auto relativeOrientationB = Cross(relativePosB, normal);

      auto angularForceA = Dot(relativeOrientationA, Multiply(inertiaA, relativeOrientationA));
      auto angularForceB = Dot(relativeOrientationB, Multiply(inertiaB, relativeOrientationB));

      auto K = invMassA + invMassB + angularForceA + angularForceB;

      auto impulse = K > 0.0f ? -C / K : 0.0f;
      auto resultantImpulse = impulse * normal;

      positionA -= invMassA * resultantImpulse;
      orientationA -= Derivative(orientationA, Multiply(inertiaA, Cross(relativePosA, resultantImpulse)));
      orientationA.Normalize();
      inertiaA = RotateToFrame(localInertiaA, ConvertQuaternionToMat33(orientationA));

      positionB += invMassB * resultantImpulse;
      orientationB += Derivative(orientationB, Multiply(inertiaB, Cross(relativePosB, resultantImpulse)));
      orientationB.Normalize();
      inertiaB = RotateToFrame(localInertiaB, ConvertQuaternionToMat33(orientationB));
    }

    positions_[contactState->indexA_] = positionA;
    orientations_[contactState->indexA_] = orientationA;
    inverseInertias_[contactState->indexA_] = inertiaA;

    positions_[contactState->indexB_] = positionB;
    orientations_[contactState->indexB_] = orientationB;
    inverseInertias_[contactState->indexB_] = inertiaB;
  }

  return minSeparation >= -3.0f * LINEAR_SLOP;
}
