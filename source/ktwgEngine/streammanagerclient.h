#pragma once
#include "istreammanager.h"

// The local client stream manager. Accepts packets from server and sends updates to server for ghost objects.
class StreamManagerClient : public IStreamManager
{
public:

    StreamManagerClient();
    ~StreamManagerClient();

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) override;

private:
     
    // Inherited via IStreamManager

    // GhostManager
    // EventManager
    // MoveManager
};