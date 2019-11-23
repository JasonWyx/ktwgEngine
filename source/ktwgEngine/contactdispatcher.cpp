#include "contactdispatcher.h"

void ContactDispatcher::AddEvent(const CollisionEvent& ce)
{
  dispatchList_.push(ce);
}

void ContactDispatcher::DispatchAll()
{
  while (!dispatchList_.empty())
  {
    auto ce = dispatchList_.front();

    ce.Dispatch();

    dispatchList_.pop();
  }
}

void ContactDispatcher::Clear()
{
  while (!dispatchList_.empty())
    dispatchList_.pop();
}
