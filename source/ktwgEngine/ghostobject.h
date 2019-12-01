#pragma once
#include <stdint.h>
#include "ghostproperty.h"
#include "ghostobjectids.h"
#include <vector>
#include <type_traits>

using GhostID = unsigned int;

struct GhostTransmissionRecord;

class GhostObject
{
public:

    GhostObject(GhostID ghostNetID);
    virtual ~GhostObject();
    
    GhostID GetGhostID() const { return m_GhostNetID; }
    size_t GetPropertyCount() const { return m_GhostProperties.size(); }
    GhostStateMask GetStateMask() const;
    GhostStateMask GetStateMaskAndCheckNeedUpdate(bool& needUpdate);
    GhostTransmissionRecord* GetLatestGhostTransmissionRecord() { return m_LatestGhostTransmissionRecord; }
    void SetLatestGhostTransmissionRecord(GhostTransmissionRecord* gtr) { m_LatestGhostTransmissionRecord = gtr; }
    void SetRetransmissionMask(const GhostStateMask& stateMask);

    bool NeedUpdate() const;
    void WriteStream(BitStream& stream, const GhostStateMask& stateMask);
    void ReadStream(BitStream& stream, const GhostStateMask& stateMask);

    template<typename T>
    void RegisterGhostPropertyImpl(T& property, NetAuthority authority = NetAuthority::Client, size_t bitSize = sizeof(T) * 8)
    {
        m_GhostProperties.push_back(new GhostPropertyVirtual<T>(property, authority, bitSize));
    }

private:

    GhostID m_GhostNetID;
    GhostTransmissionRecord* m_LatestGhostTransmissionRecord;
    std::vector<GhostProperty*> m_GhostProperties;
    GhostStateMask m_RetransmissionMask;
};