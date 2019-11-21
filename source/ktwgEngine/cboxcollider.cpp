#include "cboxcollider.h"

CBoxCollider::CBoxCollider(Entity& owner, uint32_t id)
  : Component{ owner, id }//, m_Internal{ nullptr }
{
}

CBoxCollider::~CBoxCollider()
{
}

void CBoxCollider::Initialize()
{
}

void CBoxCollider::Destroy()
{
}

bool CBoxCollider::Contains(const Vec3& point)
{

  return false;
}
