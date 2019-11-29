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

void KTWGBehaviour::Init()
{
  for (auto& elem : m_Behaviours)
    if(elem->GetActive())
      elem->Init();
}

void KTWGBehaviour::Start()
{
  for (auto& elem : m_Behaviours)
    if(elem->GetActive())
      elem->Start();
}

void KTWGBehaviour::Update()
{
  for (auto& elem : m_Behaviours)
    if(elem->GetActive())
      elem->Update();
}
