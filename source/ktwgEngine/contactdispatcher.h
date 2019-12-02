#pragma once

#include <queue>
#include "CollisionEvent.h"

class ContactDispatcher
{
public:
  void AddEvent(const CollisionEvent& ce);
  void DispatchAll();

  void Clear();

private:
  std::queue<CollisionEvent> dispatchList_;
};
