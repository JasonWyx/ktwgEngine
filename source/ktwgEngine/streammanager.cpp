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
    // Get Packet notifications

    // Process incoming packets

    // Process outgoing packets
    bool isDonePacking = false;

    while (!isDonePacking)
    {
        Packet newPacket = { m_LastPacketID++ };
        isDonePacking = PackPacket(newPacket);

        // Send using connection manager
    }
}

bool StreamManager::PackPacket(Packet& packet)
{
    TransmissionRecord& tr = m_TransmissionInfo.m_TransmissionRecords.emplace_back(TransmissionRecord{ packet.m_ID, 0 }); // Server is peer 0
    tr.m_PacketID = packet.m_ID;
    m_TransmissionRecordMap[packet.m_ID] = &tr;

    // MoveManager
    size_t packetStreamSize = packet.GetStreamSize();

    if (!m_MoveManager.WritePacket(packet))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasMove = true;

            // We have packed stream with 
            return false;
        }
    }
    else if (packet.GetStreamSize() > packetStreamSize)
    {
        packet.m_HasMove = true;
    }

    // EventManager
    packetStreamSize = packet.GetStreamSize();

    if (!m_EventManager.WritePacket(packet, m_TransmissionInfo.m_TransmissionRecords.back()))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasEvent = true;
            return false;
        }
    }
    else if (packet.GetStreamSize() > packetStreamSize)
    {
        packet.m_HasEvent = true;
    }

    // GhostManager
    packetStreamSize = packet.GetStreamSize();

    if (!m_GhostManager.WritePacket(packet, m_TransmissionInfo.m_TransmissionRecords.back()))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasGhost = true;
            return false;
        }
    }
    else if (packet.GetStreamSize() > packetStreamSize)
    {
        packet.m_HasGhost = true;
    }

    return true;
}

void StreamManager::UnpackStream(BitStream& stream)
{
    bool hasMove = false;
    bool hasEvent = false;
    bool hasGhost = false;

    stream >> hasMove >> hasEvent >> hasGhost;

    if (hasMove)
    {
        m_MoveManager.ReadStream(stream);
    }

    if (hasEvent)
    {
        m_EventManager.ReadStream(stream);
    }

    if (hasGhost)
    {
        m_GhostManager.ReadStream(stream);
    }
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
    // Get packet notification
    
    // Process incoming packets
    
    // Process outgoing packets
    std::map<PeerID, bool> isDonePacking;

    for (auto& [peerID, transmissionInfo] : m_PeerTransmissionInfos)
    {
        isDonePacking[peerID] = false;
    }

    bool isAllDonePacking = false;

    while (!isAllDonePacking)
    {
        // We do packing in round robins
        for (auto& [peerID, transmissionInfo] : m_PeerTransmissionInfos)
        {
            if (!isDonePacking[peerID])
            {
                Packet newPacket = { m_LastPacketID++ };
                isDonePacking[peerID] = PackPacket(peerID, newPacket);
                // Add message to connection manager

            }
        }

        bool isDone = true;

        for (auto& [peerID, transmissionInfo] : m_PeerTransmissionInfos)
        {
            if (!isDonePacking[peerID])
            {
                isDone = false;
            }
        }

        if (isDone)
        {
            isAllDonePacking = true;
        }
    }
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
    TransmissionRecord& tr = m_PeerTransmissionInfos[peerID].m_TransmissionRecords.emplace_back(TransmissionRecord{ packet.m_ID, peerID });
    tr.m_PacketID = packet.m_ID;
    tr.m_PeerID = peerID;
    m_TransmissionRecordMap[packet.m_ID] = &tr;

    // MoveManager
    size_t packetStreamSize = packet.GetStreamSize();

    if (!m_MoveManager.WritePacket(packet))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasMove = true;

            // We have packed stream with 
            return false;
        }
    }
    else if (packet.GetStreamSize() > packetStreamSize)
    {
        packet.m_HasMove = true;
    }

    // EventManager
    packetStreamSize = packet.GetStreamSize();

    if (!m_EventManager.WritePacket(packet, m_PeerTransmissionInfos[peerID].m_TransmissionRecords.back()))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasEvent = true;
            return false;
        }
    }
    else if (packet.GetStreamSize() > packetStreamSize)
    {
        packet.m_HasEvent = true;
    }

    // GhostManager
    packetStreamSize = packet.GetStreamSize();

    if (!m_GhostManager.WritePacket(packet, m_PeerTransmissionInfos[peerID].m_TransmissionRecords.back()))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasGhost = true;
            return false;
        }
    }
    else if (packet.GetStreamSize() > packetStreamSize)
    {
        packet.m_HasGhost = true;
    }

    return true;
}

void StreamManager::UnpackStream(PeerID peerID, BitStream& stream)
{
}

#endif

void StreamManager::InitializeInternal()
{
}

void StreamManager::ShutdownInternal()
{
}
