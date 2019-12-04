#pragma once
#include "ghostmanager.h"
#include "eventmanager.h"
#include "movemanager.h"
#include "transmissionrecord.h"
#include <vector>

// The local client stream manager. Accepts packets from server and sends updates to server for ghost objects.
class StreamManagerClient
{
public:

    StreamManagerClient();
    ~StreamManagerClient();

    void Update();
    void NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus);

    GhostManager& GetGhostManager() { return m_GhostManager; }
    EventManager& GetEventManager() { return m_EventManager; }
    MoveManager& GetMoveManager() { return m_MoveManager; }

private:

    bool ProcessIncomingPacket(Packet& packet);
    bool ProcessOutgoingPacket(Packet& packet);
     
    GhostManager m_GhostManager;
    EventManager m_EventManager;
    MoveManager m_MoveManager;

    bool m_IsDonePackingGhost;
    bool m_IsDonePackingEvent;
    bool m_IsDonePackingMove;

    std::vector<TransmissionRecord> m_TransmissionRecords;
    
    PacketID m_LastTransmittedPacket;
};