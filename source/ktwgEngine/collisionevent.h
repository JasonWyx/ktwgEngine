#pragma once

#include "collisioninfo.h"


struct CollisionEvent
{
  void Dispatch();

  Entity*       entityA_;
  Entity*       entityB_;
  CollisionInfo infoA_;
  CollisionInfo infoB_;
};