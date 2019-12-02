#include "ghostproperty.h"

BitStream& operator<<(BitStream& stream, const GhostStateMask& stateMask)
{
    for (bool state : stateMask)
    {
        stream << state;
    }
    return stream;
}

BitStream& operator>>(BitStream& stream, GhostStateMask& stateMask)
{
    for (size_t i = 0; i < stateMask.size(); ++i)
    {
        bool state = false;
        stream >> state;
        stateMask[i] = state;
    }
    return stream;
}
