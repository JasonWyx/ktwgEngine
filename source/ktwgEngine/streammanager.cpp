#include "streammanager.h"
#include <cassert>

StreamManager::StreamManager()
    : m_LastPacketID(0)
    , m_FramePacketCount(0)
    , m_MaxFramePacketCount(1000)
{
}

StreamManager::~StreamManager()
{
}

void StreamManager::Update()
{
#ifdef CLIENT
    UpdateClient();
#else
    UpdateServer();
#endif
}

void StreamManager::NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus)
{

}

Packet StreamManager::CreatePacket()
{
    return Packet{ m_LastPacketID++ };
}

#ifdef CLIENT

void StreamManager::InitializeClient()
{ 

}

void StreamManager::ShutdownClient()
{

}

void StreamManager::UpdateClient()
{
    // Process incoming packets

    // Process outgoing packets
    m_TransmissionInfo.m_IsDonePackingMove = false;
    m_TransmissionInfo.m_IsDonePackingEvent = false;
    m_TransmissionInfo.m_IsDonePackingGhost = false;

    while (m_TransmissionInfo.m_IsDonePackingMove && 
           m_TransmissionInfo.m_IsDonePackingEvent && 
           m_TransmissionInfo.m_IsDonePackingGhost)
    {
        Packet newPacket = { m_LastPacketID++ };
        PackPacket(newPacket);
    }
}

bool StreamManager::PackPacket(Packet& packet)
{
    m_TransmissionInfo.m_TransmissionRecords.push_back(TransmissionRecord{ packet.m_ID });
    m_TransmissionInfo.m_TransmissionRecords.back().m_PacketID = packet.m_ID;

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
    m_TransmissionInfo.m_IsDonePackingMove = true;

    // EventManager
    bitStreamSize = packet.m_BitStream.GetBitLength();

    if (!m_EventManager.WritePacket(packet, m_TransmissionInfo.m_TransmissionRecords.back()))
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
    m_TransmissionInfo.m_IsDonePackingEvent = true;

    // GhostManager
    bitStreamSize = packet.m_BitStream.GetBitLength();

    if (!m_GhostManager.WritePacket(packet, m_TransmissionInfo.m_TransmissionRecords.back()))
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
    m_TransmissionInfo.m_IsDonePackingGhost = true;

    return true;
}

void StreamManager::UnpackPacket(Packet& packet)
{
}


#else

void StreamManager::InitializeServer()
{

}

void StreamManager::ShutdownServer()
{

}

void StreamManager::UpdateServer()
{

}

void StreamManager::CreatePeer(PeerID peerID)
{
    auto iter = m_PeerTransmissionInfos.find(peerID);
    if (iter != m_PeerTransmissionInfos.end())
    {
        assert(false); // Trying to create a peer with an existing peer ID!
    }

    m_PeerTransmissionInfos.try_emplace(peerID);
}

void StreamManager::RemovePeer(PeerID peerID)
{
    auto iter = m_PeerTransmissionInfos.find(peerID);
    if (iter != m_PeerTransmissionInfos.end())
    {
        m_PeerTransmissionInfos.erase(iter);
    }
}

bool StreamManager::PackPacket(PeerID peerID, Packet& packet)
{
    return false;
}

void StreamManager::UnpackPacket(PeerID peerID, Packet& packet)
{
}

#endif

void StreamManager::InitializeInternal()
{
}

void StreamManager::ShutdownInternal()
{
}
