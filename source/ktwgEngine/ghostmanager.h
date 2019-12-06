#pragma once
#include "ghostobject.h"
#include "netdefs.h"
#include "packet.h"
#include "transmissionrecord.h"
#include <list>
#include <set>

class GhostManager
{
public:

    GhostManager();
    ~GhostManager();

    bool ReadStream(BitStream& stream);
    bool WriteStream(BitStream& stream, TransmissionRecord& tr, const size_t packetSizeInBits);
    void NotifyTransmissionSuccess(TransmissionRecord& tr);
    void NotifyTransmissionFailure(TransmissionRecord& tr);
    void SyncObjectPropertyValues();
    void DropPendingData();
    void ClearStatusChanges();

    void RegisterGhostObject(GhostObject* ghostObject);
    void UnregisterGhostObject(GhostObject* ghostObject);

private:

    std::vector<GhostObject*> m_GhostObjects;
    std::map<GhostID, GhostObject*> m_GhostObjectsIDMap;

    std::set<GhostID> m_GhostsToCreate;
    std::set<GhostID> m_GhostsToDelete; 

    struct PackingInfo
    {
        std::vector<std::pair<GhostObject*, GhostStateMask>> m_ObjectsToPack;
        unsigned m_LastPackedIndex;
        BitStream m_CachedObjectStream;
        bool m_IsDonePacking;
    };

#ifdef CLIENT
    PackingInfo m_PackingInfo;
#else
    std::map<PeerID, PackingInfo> m_PackingInfo;
#endif

};