#include "ccamera.h"
#include "hypegraphicsworld.h"

CCamera::CCamera(Entity & owner, uint32_t id)
:Component(typeid(CCamera), owner, id), m_Camera{nullptr}
{
}

CCamera::~CCamera()
{
}

void CCamera::Initialize()
{
  m_Camera = HypeGraphicsWorld::GetInstance().NotifyViewCreated(*GetOwner());
}

void CCamera::Destroy()
{
  HypeGraphicsWorld::GetInstance().NotifyViewDestroyed();
}
