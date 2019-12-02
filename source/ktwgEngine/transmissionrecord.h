#pragma once
#include "ghosttransmissionrecord.h"
#include "eventtransmissionrecord.h"
#include "packet.h"

struct TransmissionRecord
{
    PacketID            m_PacketID;
    std::vector<GhostTransmissionRecord> m_GhostTransmissionRecords;
    std::vector<EventTransmissionRecord> m_EventRecordList;

    bool operator<(const TransmissionRecord& rhs)
    {
        return m_PacketID < rhs.m_PacketID;
    }

    GhostTransmissionRecord* FindGhostTransmissionRecord(const GhostID netID)
    {
        for (size_t i = 0; i < m_GhostTransmissionRecords.size(); ++i)
        {
            if (m_GhostTransmissionRecords[i].m_GhostID == netID)
            {
                return &m_GhostTransmissionRecords[i];
            }
        }
        return nullptr;
    }
};