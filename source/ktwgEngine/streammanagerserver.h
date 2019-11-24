#pragma once
#include "istreammanager.h"
#include "ghostmanager.h"
#include <map>
#include <list>

class StreamManagerClient;

struct NetPeerStreamManager
{
    uint32_t m_NetPeerID;

    std::vector<TransmissionRecord> m_TransmissionRecords;

    // GhostManager
    // EventManager
    // MoveManager
};

// The server stream manager. Accepts packets from clients and broadcasts them to other connected peers.
class StreamManagerServer : public IStreamManager
{
public:

    StreamManagerServer(StreamManagerClient* streamManagerClient);
    virtual ~StreamManagerServer();

    virtual bool ProcessIncomingPacket(Packet& packet) override;
    virtual bool ProcessOutgoingPacket(Packet& packet) override;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) override;

    void AddNetPeerStreamManager(uint32_t netPeerID);
    void RemoveNetPeerStreamManager(uint32_t netPeerID);

private:

    std::map<NetPeerID, NetPeerStreamManager*> m_NetPeerStreamManagers;
    StreamManagerClient* m_LocalClientStreamManager;

};