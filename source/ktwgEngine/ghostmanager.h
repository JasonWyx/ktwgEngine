#pragma once
#include "ghostobject.h"
#include "netdefs.h"
#include "packet.h"
#include "transmissionrecord.h"
#include <list>

class GhostManager
{
public:

    GhostManager();
    ~GhostManager();

    bool ProcessIncomingPacket(Packet& packet);
    bool ProcessOutgoingPacket(Packet& packet, TransmissionRecord& transmissionRecord);
    void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus);
    void DropRemainingData();

    void RegisterGhostObject(GhostObject* ghostObject);
    void UnregisterGhostObject(GhostObject* ghostObject);

private:

    std::vector<GhostObject*> m_GhostObjects;
    std::map<GhostNetID, GhostObject*> m_GhostObjectsNetIDMap;

    // Cached for packing
    std::vector<std::pair<GhostObject*, GhostStateMask>> m_ObjectsToPack;
    unsigned m_LastPackedIndex;
    BitStream m_CachedObjectStream;
    bool m_IsDonePacking;

};