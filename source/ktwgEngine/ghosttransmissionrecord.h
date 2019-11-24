#pragma once
#include "ghostobject.h"
#include "ghostproperty.h"

struct GhostTransmissionRecord
{
    GhostID m_GhostID;
    GhostStateMask m_StateMask;
    GhostTransmissionRecord* m_NextTransmissionRecord;

    bool operator<(const GhostTransmissionRecord& rhs) const
    {
        return m_GhostID < rhs.m_GhostID;
    }
};