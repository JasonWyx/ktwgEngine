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

    void RegisterGhostID(GhostObject* ghostObject);
    void RegisterGhostObject(GhostObject* ghostObject);
    void UnregisterGhostObject(GhostObject* ghostObject);

#ifdef CLIENT
    void ReplicateToServer(GhostID ghostID);
#else
    void CreatePeer(PeerID peerID);
    void ReplicateForAllPeer(GhostID ghostID);
    void UnreplicateForAllPeer(GhostID ghostID);
    void ReplicateForPeer(PeerID targetPeerID, GhostID ghostID);
    void UnreplicateForPeer(PeerID targetPeerID, GhostID ghostID);
    void ReplicateGhostObjectsToPeer(PeerID peerID);
    void UnreplicateGhostObjectsToPeer(PeerID peerID);
    void ReplicateAllGhostsToAllPeers();
    void UnreplicateAllGhostsToAllPeers();
#endif

private:

    bool PackGhostObject(const GhostID ghostID, const PeerID peerID, BitStream& stream, GhostTransmissionRecord& gtr);

    std::vector<GhostObject*> m_GhostObjects;
    std::map<GhostID, GhostObject*> m_GhostObjectsIDMap;

    std::queue<GhostID> m_AvailableGhostIDs;

    struct PackingInfo
    {
        std::set<GhostID> m_GhostsToCreate;
        std::set<GhostID> m_GhostsToDelete; 
        std::vector<std::pair<GhostTransmissionRecord, BitStream>> m_PackedStreams;
        size_t m_PackedIndexCache = 0;

        bool m_IsDonePacking = true;
    };

#ifdef CLIENT
    PackingInfo m_PackingInfo;
#else
    std::map<PeerID, PackingInfo> m_PackingInfo;
#endif

};