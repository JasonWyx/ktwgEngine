#pragma once
#include "ghostmanager.h"
#include "eventmanager.h"
#include "transmissionrecord.h"
#include <vector>

// The local client stream manager. Accepts packets from server and sends updates to server for ghost objects.
class StreamManagerClient
{
public:

    StreamManagerClient();
    ~StreamManagerClient();

    bool ProcessIncomingPacket(Packet& packet);
    bool ProcessOutgoingPacket(Packet& packet);
    void NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus);

    GhostManager& GetGhostManager() { return m_GhostManager; }
    EventManager& GetEventManager() { return m_EventManager; }

private:
     
    GhostManager m_GhostManager;
    EventManager m_EventManager;

    bool m_IsDonePackingGhost;
    bool m_IsDonePackingEvent;

    std::vector<TransmissionRecord> m_TransmissionRecords;
};