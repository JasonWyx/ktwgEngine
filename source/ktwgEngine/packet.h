#pragma once
#include "bitstream.h"

constexpr size_t MAX_PACKET_SIZE = 512;
constexpr size_t MAX_PACKET_BIT_SIZE = 512 * 8;

using PacketID = unsigned int;

enum PacketStatus
{
    Success,
    Failed,
    Unknown
};

struct Packet
{
    PacketID m_ID = 0;
    BitStream m_BitStream;

    Packet()                            = default;
    Packet(const Packet&)               = default;
    Packet(Packet&&)                    = default;
    Packet& operator=(const Packet&)    = default;
    Packet& operator=(Packet&&)         = default;
    ~Packet()                           = default;
};