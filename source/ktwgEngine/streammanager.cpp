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
    std::vector<std::string>& incomingMessages = ConnectionManager::GetInstance().GetRecievedMessages();
    
    for (std::string& message : incomingMessages)
    {
        BitStream stream(message.size());
        std::memcpy(stream.GetData(), message.data(), stream.GetByteLength());
        UnpackStream(0, stream);
    }

    // Process outgoing packets
    bool isDonePacking = false;

    while (!isDonePacking)
    {
        Packet newPacket = { m_LastPacketID++ };
        isDonePacking = PackPacket(newPacket);

        // Send using connection manager
        BitStream finalPacketStream = newPacket.BuildStream();
        std::string message;
        message.resize(finalPacketStream.GetByteLength());
        std::memcpy(message.data(), finalPacketStream.GetData(), finalPacketStream.GetByteLength());
        ConnectionManager::GetInstance().AddPacket(message, newPacket.m_ID);
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

#else

void StreamManager::InitializeServer()
{
#ifndef CLIENT
  m_GhostManager.GenerateGhostIDs();
#endif
}

void StreamManager::ShutdownServer()
{

}

void StreamManager::UpdateServer()
{
    // Get packet notification
    
    // Process incoming packets
    std::vector<std::string>& incomingMessages = ConnectionManager::GetInstance().GetRecievedMessages();

    for (std::string& message : incomingMessages)
    {
        BitStream stream(message.size());
        std::memcpy(stream.GetData(), message.data(), stream.GetByteLength());
        UnpackStream(0, stream); // todo: peerid here is stub for now until can get peerid from incoming messages
    }
    
    // Process outgoing packets
    std::map<PeerID, bool> isDonePacking;

    // Set all transmission done to false
    for (auto& [peerID, transmissionInfo] : m_PeerTransmissionInfos)
    {
        isDonePacking[peerID] = false;
    }

    bool isAllDonePacking = false;

    while (!isAllDonePacking)
    {
        // We do packing in round robins so we don't starve anyone
        for (auto& [peerID, transmissionInfo] : m_PeerTransmissionInfos)
        {
            if (!isDonePacking[peerID])
            {
                Packet newPacket = { m_LastPacketID++ };
                isDonePacking[peerID] = PackPacket(peerID, newPacket);
                
                // Add message to connection manager
                BitStream finalPacketStream = newPacket.BuildStream();
                std::string message;
                message.resize(finalPacketStream.GetByteLength());
                std::memcpy(message.data(), finalPacketStream.GetData(), finalPacketStream.GetByteLength());
                ConnectionManager::GetInstance().AddPacket(message, newPacket.m_ID, peerID);
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

            // Synchronization of object properties is deferred until here because we want it to be the same for all peers
            m_GhostManager.SyncAllObjectProperties();
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
    tr.m_TargetPeerID = peerID;
    m_TransmissionRecordMap[packet.m_ID] = &tr;

    // MoveManager
    size_t packetStreamSize = packet.GetStreamSize();

    if (!m_MoveManager.WritePacket(packet))
    {
        if (packet.GetStreamSize() > packetStreamSize)
        {
            packet.m_HasMove = true;
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

#endif

void StreamManager::UnpackStream(const PeerID sourcePeerID, BitStream& stream)
{
    bool hasMove = false;
    bool hasEvent = false;
    bool hasGhost = false;

    stream >> hasMove >> hasEvent >> hasGhost;

    if (hasMove)
    {
        m_MoveManager.ReadStream(sourcePeerID, stream);
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

void StreamManager::InitializeInternal()
{
 
}

void StreamManager::ShutdownInternal()
{
}
