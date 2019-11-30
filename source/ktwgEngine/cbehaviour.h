#pragma once

#include <string>
#include "component.h"
#include "vector3.h"
#include "ktwgbehaviour.h"
#include "collisioneventtype.h"

struct Collision;
class Behaviour;

class CBehaviour final : public Component
{
  friend class ContactListener;
  friend struct CollisionEvent;
public:
  CBehaviour(Entity& owner, uint32_t id);
  ~CBehaviour();

  /* Override functions for abstract class */
  void Initialize() override;
  void Destroy()    override;

  Behaviour* GetInternal() const { return m_Internal; }
  
  template <typename T>
  void Bind();

  virtual void Set(Component* comp) override;

private:
  void DispatchCollisionEvent(Collision& other, const CollisionEventType& cet);

  Behaviour* m_Internal;
};

template<typename T>
inline void CBehaviour::Bind()
{
  m_Internal = KTWGBehaviour::GetInstance().CreateBehaviour<T>(*GetOwner());
}
