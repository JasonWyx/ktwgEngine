#include "streammanagerclient.h"
#include "connectionmanager.h"

StreamManagerClient::StreamManagerClient()
    : m_IsDonePackingGhost(false)
    , m_IsDonePackingEvent(false)
{
}

StreamManagerClient::~StreamManagerClient()
{
}

void StreamManagerClient::Update()
{
    // Process incoming packets

    // Process outgoing packets
    m_IsDonePackingMove = false;
    m_IsDonePackingEvent = false;
    m_IsDonePackingGhost = false;

    while (m_IsDonePackingMove || m_IsDonePackingEvent || m_IsDonePackingGhost)
    {
        Packet newPacket = { m_LastTransmittedPacket++ };
        ProcessOutgoingPacket(newPacket);
        //ConnectionManager::GetInstance().AddMessage(newPacket.m_BitStream, newPacket.m_ID);
    }
}

bool StreamManagerClient::ProcessIncomingPacket(Packet& packet)
{
    return true;
}

bool StreamManagerClient::ProcessOutgoingPacket(Packet& packet)
{
    m_TransmissionRecords.emplace_back();
    m_TransmissionRecords.back().m_PacketID = packet.m_ID;

    bool hasMoveStuff = false;
    bool hasEventStuff = false;
    bool hasGhostStuff = false;

    // MoveManager
    size_t bitStreamSize = packet.m_BitStream.GetBitLength();

    if (!m_MoveManager.WritePacket(packet))
    {
        if (packet.m_BitStream.GetBitLength() > bitStreamSize)
        {
            hasMoveStuff = true;
            return false;
        }
    }
    else if (packet.m_BitStream.GetBitLength() > bitStreamSize)
    {
        hasMoveStuff = true;
    }
    m_IsDonePackingMove = true;

    // EventManager
    bitStreamSize = packet.m_BitStream.GetBitLength();

    if (!m_EventManager.WritePacket(packet, m_TransmissionRecords.back()))
    {
        if (packet.m_BitStream.GetBitLength() > bitStreamSize)
        {
            hasEventStuff = true;
			return false;
        }
    }
    else if (packet.m_BitStream.GetBitLength() > bitStreamSize)
    {
        hasEventStuff = true;
    }
    m_IsDonePackingEvent = true;

    // GhostManager
    bitStreamSize = packet.m_BitStream.GetBitLength();

    if (!m_GhostManager.WritePacket(packet, m_TransmissionRecords.back()))
    {
        if (packet.m_BitStream.GetBitLength() > bitStreamSize)
        {
            hasGhostStuff = true;
            return false;
        }
    }
    else if (packet.m_BitStream.GetBitLength() > bitStreamSize)
    {
        hasGhostStuff = true;
    }
    m_IsDonePackingGhost = true;

	return true;
}

void StreamManagerClient::NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus)
{
}
