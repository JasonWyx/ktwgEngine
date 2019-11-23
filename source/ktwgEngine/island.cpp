#include "island.h"
#include "rigidbody.h"
#include "time.h"
#include "contactsolver.h"
#include <assert.h>

Island::Island(unsigned bodyCap, unsigned contactCap)
  : bodyCapacity_{ bodyCap }, bodyCount_{ 0 }, contactCapacity_{ contactCap }, contactCount_{ 0 }
{
  // Since we already know our worst case island. We reserve enough memories for it
  bodies_.reserve(bodyCap);
  contacts_.reserve(contactCap);

  // We only store pointers so we don't delete the actual bodies
  bodies_.resize(bodyCap);
  contacts_.resize(contactCap);

  positions_ = new Vec3[bodyCap];
  orientations_ = new Quaternion[bodyCap];
  linearVelocities_ = new Vec3[bodyCap];
  angularVelocities_ = new Vec3[bodyCap];
  inverseInertias_ = new Matrix3[bodyCap];

  allowSleep_ = true;
}

Island::~Island()
{
  delete[] positions_;
  delete[] orientations_;
  delete[] linearVelocities_;
  delete[] angularVelocities_;
  delete[] inverseInertias_;
}

void Island:: Clear()
{
  // Reset the body count to represent empty bodies
  bodyCount_ = 0;
  contactCount_ = 0;
}

void Island::AddBody(RigidBody* body)
{
  assert(bodyCount_ < bodyCapacity_);
  body->m_IslandId = bodyCount_;
  bodies_[bodyCount_] = body;
  ++bodyCount_;
}

void Island::AddContact(Contact* contact)
{
  assert(contactCount_ < contactCapacity_);
  contacts_[contactCount_] = contact;
  ++contactCount_;
}

void Island::Solve(const Vec3& external_force, float dt, unsigned vel_iteration, unsigned pos_iteration, unsigned flag)
{
  auto timeScale = Time::GetInstance().GetTimeScale();
  // Integrate velocities
  {
    //PROFILENESTEDFUNCTION(Physics, IntegrateVelocities);
    for (auto i = 0U; i < bodyCount_; ++i)
    {
      auto b = bodies_[i];

      auto v = b->m_LinearVelocity;     // velocity
      auto w = b->m_AngularVelocity;    // angular velocity
      auto position = b->m_Sweep.worldCenter_; // position
      auto orientation = b->m_Sweep.orientation_; // orientation

      // Remember position for CCD (Continuous Collision Detection)
      b->m_Sweep.worldCenterOld_ = b->m_Sweep.worldCenter_;
      b->m_Sweep.orientationOld_ = b->m_Sweep.orientation_;

      if (b->m_Type == RBT_DYNAMIC)
      {
        // Integrate linear force
        auto gravityScale = b->m_UseGravity ? b->m_GravityScale : 0.0f;

        // Add up all forces applied on body
        // Acceleration = force / mass
        auto acceleration = gravityScale * external_force + b->m_InvMass * b->m_Force;

        // Velocity = acceleration * dt
        v += acceleration * dt * timeScale;

        // Integrate rotational force
        // Angular acceleration = inverse inertia * torque
        auto angularAcc = Multiply(b->m_WorldInvInertia, b->m_Torque);

        // Angular velocity = angular acceleration * dt
        w += angularAcc * dt * timeScale;

        // Apply damping
        v *= 1.0f / (1.0f + timeScale * dt * b->m_LinearDamping);
        w *= 1.0f / (1.0f + timeScale * dt * b->m_AngularDamping);
      }

      linearVelocities_[i] = v;
      angularVelocities_[i] = w;
      positions_[i] = position;
      orientations_[i] = orientation;
      inverseInertias_[i] = b->m_WorldInvInertia;
    }
  }

  ContactSolver contactSolver{ this };

  {
    //PROFILENESTEDFUNCTION(Physics, SolveVelocityConstraint);
    contactSolver.WarmStart(dt);

    for (auto i = 0U; i < vel_iteration; ++i)
      contactSolver.SolveVelocityConstraints();

    contactSolver.StoreImpulses();
  }

  // Integrate positions
  {
    //PROFILENESTEDFUNCTION(Physics, IntegratePosition);
    for (auto i = 0U; i < bodyCount_; ++i)
    {
      auto b = bodies_[i];

      auto v = linearVelocities_[i];
      auto w = angularVelocities_[i];
      auto position = positions_[i];
      auto orientation = orientations_[i];
      auto invInertia = inverseInertias_[i];

      // Check for large velocities changes
      auto translation = v * dt * timeScale;
      if (Dot(translation, translation) > MAX_TRANSLATION_SQUARED)
      {
        auto ratio = MAX_TRANSLATION / Length(translation);
        v *= ratio;
      }

      auto rotation = w * dt * timeScale;
      if (Dot(rotation, rotation) > MAX_ROTATION_SQUARED)
      {
        auto ratio = MAX_ROTATION / Length(rotation);
        w *= ratio;
      }

      // Integrate linear positions
      position += v * dt * timeScale;

      // Integrate rotational positions
      orientation.Integrate(w, dt * timeScale);
      invInertia = RotateToFrame(b->m_InvInertia, ConvertQuaternionToMat33(orientation));

      linearVelocities_[i] = v;
      angularVelocities_[i] = w;
      positions_[i] = position;
      orientations_[i] = orientation;
      inverseInertias_[i] = invInertia;
    }
  }

  // We need to apply positional constraints to our bodies as sometimes
  // when impulse gets too huge, there will be overlaps in our velocity constraints
  // velocity constraints are only solved by acceleration.
  // So positions might overlaps due to errors in calculations
  {
    //PROFILENESTEDFUNCTION(Physics, SolvePosition)
    for (auto i = 0U; i < pos_iteration; ++i)
    {
      bool contactsSolved = contactSolver.SolvePositionConstraints();

      // If our position constraints' errors are small enough, we can break early
      if (contactsSolved)
        break;
    }
  }

  {
    //PROFILENESTEDFUNCTION(Physics, CopyStateBuffer);
    // Copy state buffers back into body
    for (auto i = 0U; i < bodyCount_; ++i)
    {
      RigidBody* b = bodies_[i];

      if (b->GetIgnorePhysics())
        return;

      b->m_Sweep.worldCenter_ = positions_[i];
      b->m_Sweep.orientation_ = orientations_[i];
      b->m_Sweep.orientation_.Normalize();
      b->m_LinearVelocity = linearVelocities_[i];
      b->m_AngularVelocity = angularVelocities_[i];
      b->m_WorldInvInertia = inverseInertias_[i];

      b->m_Force.SetZero();
      b->m_Torque.SetZero();
      b->SynchroniseTransform();
    }
  }

  {
    //PROFILENESTEDFUNCTION(Physics, Sleep);
    if (flag & IF_SLEEP)
    {
      float minSleepTime = std::numeric_limits<float>::max();
      for (auto i = 0U; i < bodyCount_; ++i)
      {
        RigidBody* b = bodies_[i];
        if (b->m_Type == RBT_STATIC)
          continue;

        // Compute the linear and angular speed of the body.
        float sqrLinVel = Dot(b->m_LinearVelocity, b->m_LinearVelocity);
        float sqrAngVel = Dot(b->m_AngularVelocity, b->m_AngularVelocity);

        if (sqrLinVel > SLEEP_LINEAR_TOL || sqrAngVel > SLEEP_ANGULAR_TOL)
        {
          minSleepTime = 0.0f;
          b->m_SleepTime = 0.0f;
        }
        else
        {
          b->m_SleepTime += dt * timeScale;
          minSleepTime = Math::Min(minSleepTime, b->m_SleepTime);
        }
      }

      // Put the island to sleep so long as the minimum found sleep time
      // is below the threshold. 
      if (minSleepTime >= TIME_TO_SLEEP)
      {
        for (auto i = 0U; i < bodyCount_; ++i)
          bodies_[i]->SetAwake(false);
      }
    }
  }
}
