#pragma once
#include <stdint.h>
#include "vector3.h"

#ifndef PI
#define PI 3.14159265359f
#endif

constexpr unsigned MAX_COLLISION_LAYER = 32;  // Max collision layer

constexpr uint32_t MAX_RIGIDBODIES = 1000;

constexpr uint32_t INVALID_ISLAND_ID = UINT32_MAX;

constexpr float DEFAULT_MASS = 1.0f;

constexpr uint32_t ALL_LAYER = 4294967295UL;

// A UNIT EXTENTS
const Vec3 UNIT_EXTENTS{ 0.5f, 0.5f, 0.5f };

// Physics Solver constants
constexpr auto LINEAR_SLOP = 0.0005f;
constexpr auto ANGULAR_SLOP = 2.0f / 180.0f * PI;
constexpr auto PENETRATION_SLOP = 0.05f;
constexpr auto BAUMGARTE = 0.2f;
constexpr auto MAX_LINEAR_CORRECTION = 0.2f;

constexpr float AABB_EXTENSION = 0.2f;
constexpr float AABB_MULTIPLIER = 2.0f;

// BASIS VECTORS
const Vec3 X_VECTOR{ 1.0f, 0.0f, 0.0f };
const Vec3 Y_VECTOR{ 0.0f, 1.0f, 0.0f };
const Vec3 Z_VECTOR{ 0.0f, 0.0f, 1.0f };

// Sleep threshold for island simulation
constexpr auto SLEEP_LINEAR_TOL = 0.05f;
constexpr auto SLEEP_ANGULAR_TOL = 0.001f;
constexpr auto TIME_TO_SLEEP = 0.2;

// Maximum translation threshold in physics
constexpr auto MAX_TRANSLATION = 2.0f;
constexpr auto MAX_TRANSLATION_SQUARED = MAX_TRANSLATION * MAX_TRANSLATION;
constexpr auto MAX_ROTATION = 0.5f * PI;
constexpr auto MAX_ROTATION_SQUARED = MAX_ROTATION * MAX_ROTATION;