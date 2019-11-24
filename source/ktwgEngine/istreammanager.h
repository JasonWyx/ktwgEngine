#pragma once
#include "bitstream.h"
#include "netdefs.h"
#include "packet.h"

class IStreamManager
{
public:

    virtual bool ProcessIncomingPacket(Packet& packet) = 0;
    virtual bool ProcessOutgoingPacket(Packet& packet) = 0;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) = 0;
};