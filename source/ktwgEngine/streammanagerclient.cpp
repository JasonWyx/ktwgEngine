#include "streammanagerclient.h"

StreamManagerClient::StreamManagerClient()
    : m_IsDonePackingGhost(false)
    , m_IsDonePackingEvent(false)
{
}

StreamManagerClient::~StreamManagerClient()
{
}

bool StreamManagerClient::ProcessIncomingPacket(Packet& packet)
{
    return true;
}

bool StreamManagerClient::ProcessOutgoingPacket(Packet& packet)
{
    // For the client we write directly to the packet because we do not need to replicate the packet
    m_TransmissionRecords.emplace_back();
    m_TransmissionRecords.back().m_PacketID = packet.m_ID;
    
    if (!m_IsDonePackingGhost && 
        !m_GhostManager.WriteStream(packet.m_BitStream, m_TransmissionRecords.back()))
    {
        return false;
    }
    else
    {
        m_IsDonePackingGhost = true;
    }

    /*if (!m_IsDonePackingEvent &&
        !m_EventManager.WritePacketBitStream(packet.m_BitStream, m_TransmissionRecords.back()))
    {
        return false;
    }
    else
    {
        m_IsDonePackingEvent = true;
    }*/

    m_IsDonePackingGhost = false;
    m_IsDonePackingEvent = false;

    return true;
}

void StreamManagerClient::NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus)
{
}
