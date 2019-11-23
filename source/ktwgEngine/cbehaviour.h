#pragma once

#include <string>
#include "component.h"
#include "vector3.h"
#include "ktwgbehaviour.h"

class Behaviour;

class CBehaviour final : public Component
{
public:
  CBehaviour(Entity& owner, uint32_t id);
  ~CBehaviour();

  /* Override functions for abstract class */
  void Initialize() override;
  void Destroy()    override;

  Behaviour* GetInternal() const { return m_Internal; }
  
  template <typename T>
  void Bind();

private:
  Behaviour* m_Internal;
};

template<typename T>
inline void CBehaviour::Bind()
{
  m_Internal = KTWGBehaviour::GetInstance().CreateBehaviour<T>(*GetOwner());
}
