#pragma once

class Entity;
class CBoxCollider;
class CRigidBody;
class Transform;

struct Collision
{
  CBoxCollider* collider;
  CRigidBody*   rigidBody;
  Entity*      entity;
  Transform*   transform;
};
