#pragma once
#include "ghosttransmissionrecord.h"
#include "packet.h"
#include "event.h"
#include <list>

struct TransmissionRecord
{
    PacketID m_PacketID;
    PeerID m_TargetPeerID;

    std::list<GhostTransmissionRecord> m_GhostTransmissionRecords;
    std::list<Event*> m_Events;

    bool operator<(const TransmissionRecord& rhs)
    {
        return m_PacketID < rhs.m_PacketID;
    }
};