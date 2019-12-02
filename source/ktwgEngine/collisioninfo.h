#pragma once

class Entity;

/* Wraps any collision informations that can be sent to the collision events for usage */
struct CollisionInfo
{
  Entity* other_;
};
