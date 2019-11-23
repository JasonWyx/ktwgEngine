#pragma once

#include "vector3.h"

class BoxCollider;
class Entity;

struct RayCastInput
{
  Vec3     m_p;           // starting point of raycast
  Vec3     m_q;           // ending point of raycast
  uint32_t m_LayerMask;   // Layer mask stating if the collider should be detected with ray cast
  float    m_MaxT = 1.0f; // maximum time of intersection
};

struct RayCastOutput
{
  RayCastOutput()
    : m_Point{}, m_Normal{}, m_Collider{ nullptr }, m_Owner{ nullptr }, m_T{ FLT_MAX }
  {

  }

  void Reset()
  {
    m_Collider = nullptr;
    m_Normal   = Vec3{};
    m_Point    = Vec3{};
    m_Owner    = nullptr;
    m_T        = FLT_MAX;
  }

  Vec3         m_Point;    // point of intersection of raycast
  Vec3         m_Normal;   // normal of the raycast hit
  BoxCollider* m_Collider; // hit collider
  Entity*      m_Owner;    // owner of the collider
  float        m_T;        // time of intersection
};
