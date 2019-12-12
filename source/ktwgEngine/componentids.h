#pragma once
#include "bitstream.h"

// Jason: I wanted to use GhostObjectID but VS was complaining about invalid tag enum
enum ComponentID
{
    CI_Transform,
    CI_Component,
    CI_Entity,
    CI_Count
};