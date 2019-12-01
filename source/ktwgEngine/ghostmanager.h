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
    bool WriteStream(BitStream& stream, TransmissionRecord& tr);
    void NotifyTransmissionSuccess(TransmissionRecord& tr);
    void NotifyTransmissionFailure(TransmissionRecord& tr);
    void DropPendingStreamData();

    void RegisterGhostObject(GhostObject* ghostObject);
    void UnregisterGhostObject(GhostObject* ghostObject);

private:

    std::vector<GhostObject*> m_GhostObjects;
    std::map<GhostID, GhostObject*> m_GhostObjectsIDMap;
    std::set<GhostID> m_GhostsToCreate;
    std::set<GhostID> m_GhostsToDelete; 

    // Cached for packing
    std::vector<std::pair<GhostObject*, GhostStateMask>> m_ObjectsToPack;
    unsigned m_LastPackedIndex;
    BitStream m_CachedObjectStream;
    bool m_IsDonePacking;

};
