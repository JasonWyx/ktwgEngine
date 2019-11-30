#include "cbehaviour.h"
#include "ktwgbehaviour.h"
#include "behaviour.h"

CBehaviour::CBehaviour(Entity& owner, uint32_t id)
  : Component{ typeid(CBehaviour), owner, id }, m_Internal { nullptr }
{
  SetType(CT_BEHAVIOUR);
}

CBehaviour::~CBehaviour()
{
}

void CBehaviour::Initialize()
{
}

void CBehaviour::Destroy()
{
}

void CBehaviour::Set(Component* comp)
{
  Component::Set(comp);

  CBehaviour* behComp = static_cast<CBehaviour*>(comp);

  if (m_Internal && behComp->m_Internal)
    m_Internal->Set(behComp->m_Internal);
}

void CBehaviour::DispatchCollisionEvent(Collision& other, const CollisionEventType& cet)
{
  switch (cet)
  {
  case CET_ONCOLLISIONENTER:
    m_Internal->OnCollisionEnter(other);
    break;
  case CET_ONCOLLISIONSTAY:
    m_Internal->OnCollisionStay(other);
    break;
  case CET_ONCOLLISIONEXIT:
    m_Internal->OnCollisionExit(other);
    break;
  case CET_ONTRIGGERENTER:
    m_Internal->OnTriggerEnter(other);
    break;
  case CET_ONTRIGGERSTAY:
    m_Internal->OnTriggerStay(other);
    break;
  case CET_ONTRIGGEREXIT:
    m_Internal->OnTriggerExit(other);
    break;
  };
}
