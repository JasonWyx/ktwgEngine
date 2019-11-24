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
    
    GhostID GetGhostNetID() const { return m_GhostNetID; }
    size_t GetPropertyCount() const { return m_GhostProperties.size(); }
    GhostStateMask GetStateMask() const;
    GhostStateMask GetStateMaskAndCheckNeedUpdate(bool& needUpdate);
    GhostTransmissionRecord* GetLatestGhostTransmissionRecord() { return m_LatestGhostTransmissionRecord; }
    void SetLatestGhostTransmissionRecord(GhostTransmissionRecord* latestGhostTransmissionRecord) { m_LatestGhostTransmissionRecord = latestGhostTransmissionRecord; }

    bool NeedUpdate() const;
    void WriteStream(BitStream& stream, const GhostStateMask& stateMask);
    void ReadStream(BitStream& stream, const GhostStateMask& stateMask);

    template<typename T>
    void RegisterGhostPropertyImpl(T& property)
    {
        m_GhostProperties.push_back(MakeGhostProperty(property));
    }

#define RegisterGhostProperty(property, bitCount) RegisterGhostPropertyImpl<decltype(property), N>(property)

private:

    GhostID m_GhostNetID;
    GhostTransmissionRecord* m_LatestGhostTransmissionRecord;
    std::vector<GhostProperty*> m_GhostProperties;
};