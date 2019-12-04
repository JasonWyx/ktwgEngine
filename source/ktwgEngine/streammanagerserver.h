#pragma once
#include <map>
#include <list>
#include <vector>
#include "transmissionrecord.h"
#include "netdefs.h"

class StreamManagerClient;

// The server stream manager. Accepts packets from clients and broadcasts them to other connected peers.
class StreamManagerServer
{
public:

    StreamManagerServer(StreamManagerClient* streamManagerClient);
    ~StreamManagerServer();

    // We only have outgoing packet here because we let the incoming packet be handled by the client stream manager
    void Update();
    bool SendPacket(PeerID targetSourceID, Packet& packet);
    void NotifyPacketStatus(PeerID peerID, PacketID packetID, PacketStatus packetStatus);

    void CreatePeer(PeerID peerID);
    void RemovePeer(PeerID peerID);

private:

    std::map<PeerID, std::vector<TransmissionRecord>> m_PeerTransmissionRecords;

    StreamManagerClient* m_LocalClientStreamManager;

};