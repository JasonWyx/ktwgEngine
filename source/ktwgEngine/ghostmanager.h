#pragma once
#include "ghostobject.h"
#include "netdefs.h"
#include "packet.h"
#include "transmissionrecord.h"
#include <list>
#include <set>
#include <queue>

class GhostManager
{
public:

    GhostManager();
    ~GhostManager();

    void GenerateGhostIDs();
    GhostID GetAvailableGhostID();

    void ReadStream(BitStream& stream);
    bool WritePacket(Packet& packet, TransmissionRecord& tr);
    void NotifyTransmissionSuccess(TransmissionRecord& tr);
    void NotifyTransmissionFailure(TransmissionRecord& tr);
    void SyncAllObjectProperties();
    void DropPendingData();
    void ClearStatusChanges();

    void RegisterGhostObject(GhostObject* ghostObject);
    void UnregisterGhostObject(GhostObject* ghostObject);

#ifdef CLIENT
    void ReplicateToServer(GhostID ghostID);
#else
    void ReplicateForAllPeer(GhostID ghostID);
    void UnreplicateForAllPeer(GhostID ghostID);
    void ReplicateForPeer(PeerID targetPeerID, GhostID ghostID);
    void UnreplicateForPeer(PeerID targetPeerID, GhostID ghostID);
#endif

private:

    std::vector<GhostObject*> m_GhostObjects;
    std::map<GhostID, GhostObject*> m_GhostObjectsIDMap;

    std::queue<GhostID> m_AvailableGhostIDs;

    struct PackingInfo
    {
        std::vector<std::pair<GhostID, GhostStateMask>> m_ObjectsToPack;
        unsigned m_LastPackedIndex;
        BitStream m_CachedObjectStream;
        GhostTransmissionRecord m_CachedTransmissionRecord;
        bool m_IsDonePacking;

        std::set<GhostID> m_GhostsToCreate;
        std::set<GhostID> m_GhostsToDelete; 
    };

#ifdef CLIENT
    PackingInfo m_PackingInfo;
#else
    std::map<PeerID, PackingInfo> m_PackingInfo;
#endif

};