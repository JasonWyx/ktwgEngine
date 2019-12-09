#pragma once
#include <array>

enum MoveStateFlags
{
    Up = 0,
    Down,
    Left,
    Right,
    NbMoveStateFlags
};

using MoveState = std::array<bool, MoveStateFlags::NbMoveStateFlags>;