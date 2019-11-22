#pragma once
#include <stdint.h>
#include "vector3.h"

constexpr uint32_t MAX_RIGIDBODIES = 1000;

constexpr uint32_t INVALID_ISLAND_ID = UINT32_MAX;

constexpr float DEFAULT_MASS = 1.0f;

// A UNIT EXTENTS
const Vec3 UNIT_EXTENTS{ 0.5f, 0.5f, 0.5f };