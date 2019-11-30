#pragma once

#include "collisioninfo.h"
#include "collisioneventtype.h"


struct CollisionEvent
{
  void Dispatch();

  Entity*       entityA_;
  Entity*       entityB_;
  CollisionInfo infoA_;
  CollisionInfo infoB_;

  CollisionEventType type_;
};