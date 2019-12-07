#pragma once
#include "ghosttransmissionrecord.h"
#include "eventtransmissionrecord.h"
#include "packet.h"

struct TransmissionRecord
{
    PacketID m_PacketID;
    PeerID m_TargetPeerID;

    std::vector<GhostTransmissionRecord> m_GhostTransmissionRecords;
    std::vector<EventTransmissionRecord> m_EventTransmissionRecords;

    bool operator<(const TransmissionRecord& rhs)
    {
        return m_PacketID < rhs.m_PacketID;
    }
};