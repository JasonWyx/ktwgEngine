#pragma once
#include "bitstream.h"

constexpr size_t MAX_PACKET_SIZE = 512;
constexpr size_t MAX_PACKET_BIT_SIZE = 512 * 8;

using PacketID = unsigned char;

enum class PacketStatus
{
    Success,
    Failed,
    Unknown
};

struct Packet
{
    PacketID m_ID = 0;

    bool m_HasMove = false;
    bool m_HasEvent = false;
    bool m_HasGhost = false;

    BitStream m_MoveStream;
    BitStream m_EventStream;
    BitStream m_GhostStream;

    BitStream BuildStream() const
    {
        BitStream result;

        result << m_HasMove << m_HasEvent << m_HasGhost;

        if (m_MoveStream.GetBitLength() > 0)
        {
            result += m_MoveStream;
        }
        if (m_EventStream.GetBitLength() > 0)
        {
            result += m_EventStream;
        }
        if (m_GhostStream.GetBitLength() > 0)
        {
            result += m_GhostStream;
        }

        return result;
    }

    size_t GetStreamSize() const
    {
        return 3 + m_MoveStream.GetBitLength() + m_EventStream.GetBitLength() + m_GhostStream.GetBitLength();
    }
};