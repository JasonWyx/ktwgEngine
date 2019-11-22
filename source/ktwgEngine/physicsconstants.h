#pragma once
#include <stdint.h>
#include "vector3.h"

constexpr uint32_t MAX_RIGIDBODIES = 1000;

constexpr uint32_t INVALID_ISLAND_ID = UINT32_MAX;

constexpr float DEFAULT_MASS = 1.0f;

// A UNIT EXTENTS
const Vec3 UNIT_EXTENTS{ 0.5f, 0.5f, 0.5f };

// Physics Solver constants
constexpr auto LINEAR_SLOP = 0.0005f;
constexpr auto ANGULAR_SLOP = 2.0f / 180.0f * PI;
constexpr auto PENETRATION_SLOP = 0.05f;
constexpr auto BAUMGARTE = 0.2f;
constexpr auto MAX_LINEAR_CORRECTION = 0.2f;
