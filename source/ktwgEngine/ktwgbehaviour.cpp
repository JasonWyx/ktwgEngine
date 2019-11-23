#include "KTWGBehaviour.h"
#include "behaviour.h"

KTWGBehaviour::KTWGBehaviour()
  : m_Behaviours{}
{
}

KTWGBehaviour::~KTWGBehaviour()
{
}

void KTWGBehaviour::InitializeInternal()
{
}

void KTWGBehaviour::ShutdownInternal()
{
}

void KTWGBehaviour::Update()
{
  for (auto& elem : m_Behaviours)
    elem->Update();
}
