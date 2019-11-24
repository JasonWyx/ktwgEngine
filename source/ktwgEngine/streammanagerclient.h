#pragma once
#include "istreammanager.h"
#include "ghostmanager.h"

// The local client stream manager. Accepts packets from server and sends updates to server for ghost objects.
class StreamManagerClient : public IStreamManager
{
public:

    StreamManagerClient();
    ~StreamManagerClient();

    virtual bool ProcessIncomingPacket(Packet& stream) override;
    virtual bool ProcessOutgoingPacket(Packet& stream) override;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) override;

    GhostManager& GetGhostManager() { return m_GhostManager; }

private:
     
    GhostManager m_GhostManager;
};