#pragma once
#include <stdint.h>
#include "ghostproperty.h"
#include "ghostobjectids.h"
#include <vector>
#include <type_traits>
#include "netdefs.h"

using GhostID = unsigned int;

struct GhostTransmissionRecord;

class GhostObject
{
public:

    GhostObject(GhostID ghostNetID, PeerID owner);
    virtual ~GhostObject();
    
    GhostID GetGhostID() const { return m_GhostID; }
    size_t GetPropertyCount() const { return m_GhostProperties.size(); }
    GhostStateMask GetStateMask() const;
    GhostStateMask GetStateMaskAndCheckNeedUpdate(bool& needUpdate);
    GhostTransmissionRecord* GetLatestGhostTransmissionRecord() { return m_LatestGhostTransmissionRecord; }
    void SetLatestGhostTransmissionRecord(GhostTransmissionRecord* gtr) { m_LatestGhostTransmissionRecord = gtr; }
    void SetRetransmissionMask(const GhostStateMask& stateMask);

    bool NeedUpdate() const;
    void WriteStream(BitStream& stream, const GhostStateMask& stateMask);
    void ReadStream(BitStream& stream, const GhostStateMask& stateMask);
    
    inline bool IsOwner();

    template<typename T>
    void RegisterGhostProperty(T& property, NetAuthority authority, size_t bitCount = sizeof(T) * 8)
    {
        m_GhostProperties.push_back(new GhostPropertyVirtual<T>(property, authority, bitCount));
    }


private:

    PeerID m_Owner;
    GhostID m_GhostID;
    GhostTransmissionRecord* m_LatestGhostTransmissionRecord;
    std::vector<GhostProperty*> m_GhostProperties;
    
    GhostStateMask m_RetransmissionMask;
    GhostStateMask m_ServerTransmissionMask;
};