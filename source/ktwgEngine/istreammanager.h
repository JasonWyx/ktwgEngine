#pragma once
#include "bitstream.h"

using NetPeerID     = unsigned int;
using PacketID      = unsigned int;
using PacketStatus  = unsigned int;

class IStreamManager
{
public:

    virtual bool ProcessIncomingPacket(BitStream& stream) = 0;
    virtual bool ProcessOutgoingPacket(BitStream& steram) = 0;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) = 0;
};