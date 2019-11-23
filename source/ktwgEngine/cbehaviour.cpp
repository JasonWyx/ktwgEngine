#include "cbehaviour.h"
#include "ktwgbehaviour.h"

CBehaviour::CBehaviour(Entity& owner, uint32_t id)
  : Component{ typeid(CBehaviour), owner, id }, m_Internal { nullptr }
{
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
