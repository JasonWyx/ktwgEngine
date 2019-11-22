#include "cboxcollider.h"
#include "boxcollider.h"
#include <assert.h>

CBoxCollider::CBoxCollider(Entity& owner, uint32_t id)
  : Component{ owner, id }, m_Internal{ nullptr }
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

Vec3 CBoxCollider::GetSize() const
{
  assert(m_Internal);

  return m_Internal->GetExtents() * 2.0f;
}

Vec3 CBoxCollider::GetCenter() const
{
  assert(m_Internal);

  return m_Internal->GetCenter();
}

const Vec3& CBoxCollider::GetMin() const
{
  assert(m_Internal);

  return m_Internal->GetMin();
}

const Vec3& CBoxCollider::GetMax() const
{
  assert(m_Internal);

  return m_Internal->GetMax();
}

bool CBoxCollider::GetIsTrigger() const
{
  assert(m_Internal);
  
  return m_Internal->GetIsTrigger();
}

bool CBoxCollider::GetActive() const
{
  assert(m_Internal);

  return m_Internal->GetActive();
}

void CBoxCollider::SetSize(const Vec3& size)
{
  assert(m_Internal);

  m_Internal->SetExtents(size * 0.5f);
}

void CBoxCollider::SetCenter(const Vec3& center)
{
  assert(m_Internal);

  m_Internal->SetCenter(center);
}

void CBoxCollider::SetIsTrigger(bool isTrigger)
{
}

void CBoxCollider::SetActive(bool active)
{
  m_Internal->SetActive(active);
}
