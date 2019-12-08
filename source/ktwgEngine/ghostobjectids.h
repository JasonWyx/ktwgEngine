#pragma once
#include "bitstream.h"

// Jason: I wanted to use GhostObjectID but VS was complaining about invalid tag enum
enum ClassID
{
    CI_Transform,
    CI_Component,
    SomeClass2,
    SomeClass3,
    SomeClass4,
    SomeClass5,
    Count
};

constexpr size_t BitsRequired(size_t n)
{
    size_t bitCount = 0;

    while (n > 0)
    {
        n >>= 1;
        bitCount++;
    }
    
    return bitCount;
}

constexpr size_t GhostObjectIDSize = BitsRequired(ClassID::Count);

//// Jason: Helper struct to convert ID to int and use BitStream for minimum bit use
//struct GhostClassID
//{
//    size_t m_ID;
//
//    operator size_t() const { return m_ID; }
//};
//
//BitStream& operator<<(BitStream& bs, const GhostClassID& ghostObjectID)
//{
//    return bs.Write(ghostObjectID.m_ID, GhostObjectIDSize);
//}
//
//BitStream& operator>>(BitStream& bs, GhostClassID& ghostObjectID)
//{
//    return bs.Read(ghostObjectID.m_ID, GhostObjectIDSize);
//}

