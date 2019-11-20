#include "rigidbody.h"

RigidBody::RigidBody(const Entity& entity)
  : m_Entity{ &entity }, m_Type{ RBT_DYNAMIC }, m_Mass{ 1.0f }, m_InvMass{ 1.0f },
    m_LinearDamping{}, m_AngularDamping{ 0.5f }, m_GravityScale{ 1.0f },
    m_UseGravity{ true }, m_IslandId{ INVALID_ISLAND_ID }
{
}
