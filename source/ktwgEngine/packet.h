#pragma once
#include "bitstream.h"
#include "netdefs.h"

constexpr size_t MAX_PACKET_SIZE = 400;
constexpr size_t MAX_PACKET_BIT_SIZE = MAX_PACKET_SIZE * 8;

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
    PeerID m_TargetPeerID = 0;

    BitStream m_MoveStream;
    BitStream m_EventStream;
    BitStream m_GhostStream;

    BitStream BuildStream() const
    {
        BitStream result;

        result << (m_MoveStream.GetByteLength() > 0)
               << (m_EventStream.GetByteLength() > 0)
               << (m_GhostStream.GetByteLength() > 0);

        if (m_MoveStream.GetByteLength() > 0)
        {
            result += m_MoveStream;
        }
        if (m_EventStream.GetByteLength() > 0)
        {
            result += m_EventStream;
        }
        if (m_GhostStream.GetByteLength() > 0)
        {
            result += m_GhostStream;
        }

        return result;
    }

    size_t GetStreamSize() const
    {
        return 3 + m_MoveStream.GetBitPosition() + m_EventStream.GetBitPosition() + m_GhostStream.GetBitPosition();
    }

    bool HasContents() const
    {
        return (m_MoveStream.GetByteLength() > 0) && (m_EventStream.GetByteLength() > 0) && (m_GhostStream.GetByteLength() > 0);
    }
};