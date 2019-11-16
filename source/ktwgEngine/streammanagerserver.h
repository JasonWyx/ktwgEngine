#pragma once
#include "istreammanager.h"
#include <map>
#include <list>

class GhostTransmissionRecord;
class EventTransmissionRecord;
class StreamManagerClient;

struct NetPeerStreamManager
{
    uint32_t m_NetPeerID;

    std::list<GhostTransmissionRecord> m_GhostTransmissionRecords;
    std::list<EventTransmissionRecord> m_EventTransmissionRecords;

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

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) override;

    void AddNetPeerStreamManager(uint32_t netPeerID);
    void RemoveNetPeerStreamManager(uint32_t netPeerID);

private:

    std::map<NetPeerID, NetPeerStreamManager*> m_NetPeerStreamManagers;
    StreamManagerClient* m_LocalClientStreamManager;

};