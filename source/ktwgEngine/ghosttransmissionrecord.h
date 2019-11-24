#pragma once

struct GhostTransmissionRecord
{
    GhostNetID m_NetID;
    GhostStateMask m_StateMask;
    GhostTransmissionRecord* m_NextTransmissionRecord;

    bool operator<(const GhostTransmissionRecord& rhs) const
    {
        return m_NetID < rhs.m_NetID;
    }
};