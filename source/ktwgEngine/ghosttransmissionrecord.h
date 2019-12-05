#pragma once
#include "ghostobject.h"
#include "ghostproperty.h"

enum class GhostStatusType
{
    None = 0,
    Create,
    Delete
};

struct GhostTransmissionRecord
{
    GhostID m_GhostID;
    GhostStateMask m_StateMask;
    GhostStatusType m_Status;
    GhostTransmissionRecord* m_NextTransmissionRecord;
    GhostTransmissionRecord* m_PreviousTransmissionRecord;

    bool operator<(const GhostTransmissionRecord& rhs) const
    {
        return m_GhostID < rhs.m_GhostID;
    }
};