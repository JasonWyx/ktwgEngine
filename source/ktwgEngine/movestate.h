#pragma once
#include <array>

enum MoveStateFlags
{
    Up = 0,
    Down,
    Left,
    Right,
    Count
};

using MoveState = std::array<bool, MoveStateFlags::Count>;