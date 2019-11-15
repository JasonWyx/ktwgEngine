#pragma once
#include "bitstream.h"

class IStreamManager
{
    virtual bool ProcessIncomingPacket(BitStream& stream) = 0;
    virtual bool ProcessOutgoingPacket(BitStream& stream) = 0;
    virtual void NotifyPacketStatus(uint32_t packetID, uint32_t packetStatus) = 0;
};