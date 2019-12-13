#include "streammanager.h"
#include "scene.h"
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
    std::vector<int>& lostPackets = ConnectionManager::GetInstance().GetLostPacketIDs();

    for (int lostPacketID : lostPackets)
    {
        TransmissionRecord* tr = m_TransmissionRecordMap[lostPacketID];
        if (!tr->m_GhostTransmissionRecords.empty())
        {
            m_GhostManager.NotifyTransmissionFailure(*tr);
        }
        if (!tr->m_EventTransmissionRecords.empty())
        {
            m_EventManager.NotifyTransmissionFailure(*tr);
        }
        m_TransmissionRecordMap.erase(lostPacketID);

        m_TransmissionInfo.m_TransmissionRecords.remove_if([lostPacketID](TransmissionRecord& transmissionRecord)
        {
            return transmissionRecord.m_PacketID == lostPacketID;
        });
    }

    lostPackets.clear();

    auto& successPackets = ConnectionManager::GetInstance().GetAckPacketIDs();

    for (auto&[peerID, transmissionRecords] : successPackets)
    {
        for (int lostPacketID : lostPackets)
        {
            TransmissionRecord* tr = m_TransmissionRecordMap[lostPacketID];
            if (!tr->m_GhostTransmissionRecords.empty())
            {
                m_GhostManager.NotifyTransmissionSuccess(*tr);
            }
            if (!tr->m_EventTransmissionRecords.empty())
            {
                m_EventManager.NotifyTransmissionSuccess(*tr);
            }
            m_TransmissionRecordMap.erase(lostPacketID);

            m_TransmissionInfo.m_TransmissionRecords.remove_if([lostPacketID](TransmissionRecord& transmissionRecord)
            {
                return transmissionRecord.m_PacketID == lostPacketID;
            });
        }
    }

    // Process incoming packets
    std::vector<std::vector<unsigned char>>& incomingMessages = ConnectionManager::GetInstance().GetRecievedMessages();
    
    for (std::vector<unsigned char>& message : incomingMessages)
    {
        BitStream stream(message.size());
        std::memcpy(stream.GetData(), message.data(), stream.GetByteLength());
        UnpackStream(0, stream);
    }

    incomingMessages.clear();

    // Process outgoing packets
    bool isDonePacking = false;

    while (!isDonePacking)
    {
        Packet newPacket = { m_LastPacketID++ };
        isDonePacking = PackPacket(newPacket);

        if (!newPacket.HasContents())
        {
            continue;
        }

        // Send using connection manager
        BitStream finalPacketStream = newPacket.BuildStream();
        std::vector<unsigned char> message;
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

    bool didAllSucceed = true;

    didAllSucceed = didAllSucceed && m_MoveManager.WritePacket(packet);
    didAllSucceed = didAllSucceed && m_EventManager.WritePacket(packet, m_TransmissionInfo.m_TransmissionRecords.back());
    didAllSucceed = didAllSucceed && m_GhostManager.WritePacket(packet, m_TransmissionInfo.m_TransmissionRecords.back());

    return didAllSucceed;
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
    // Get Packet notifications
    std::vector<int>& lostPackets = ConnectionManager::GetInstance().GetLostPacketIDs();

    for (int lostPacketID : lostPackets)
    {
        TransmissionRecord* tr = m_TransmissionRecordMap[lostPacketID];
        if (!tr->m_GhostTransmissionRecords.empty())
        {
            m_GhostManager.NotifyTransmissionFailure(*tr);
        }
        if (!tr->m_EventTransmissionRecords.empty())
        {
            m_EventManager.NotifyTransmissionFailure(*tr);
        }
        m_TransmissionRecordMap.erase(lostPacketID);

        m_PeerTransmissionInfos[tr->m_TargetPeerID].m_TransmissionRecords.remove_if([lostPacketID](TransmissionRecord& transmissionRecord)
        {
            return transmissionRecord.m_PacketID == lostPacketID;
        });
    }

    lostPackets.clear();

    auto& successPackets = ConnectionManager::GetInstance().GetAckPacketIDs();

    for (auto&[peerID, transmissionRecords] : successPackets)
    {
        for (int lostPacketID : lostPackets)
        {
            TransmissionRecord* tr = m_TransmissionRecordMap[lostPacketID];
            if (!tr->m_GhostTransmissionRecords.empty())
            {
                m_GhostManager.NotifyTransmissionSuccess(*tr);
            }
            if (!tr->m_EventTransmissionRecords.empty())
            {
                m_EventManager.NotifyTransmissionSuccess(*tr);
            }
            m_TransmissionRecordMap.erase(lostPacketID);

            m_PeerTransmissionInfos[tr->m_TargetPeerID].m_TransmissionRecords.remove_if([lostPacketID](TransmissionRecord& transmissionRecord)
            {
                return transmissionRecord.m_PacketID == lostPacketID;
            });
        }
    }

    // Process incoming packets
    std::map<int, std::vector<std::vector<unsigned char>>>& incomingMessages = ConnectionManager::GetInstance().GetRecievedMessages();

    for (auto&[peerID, messages] : incomingMessages)
    {
      for (auto& message : messages)
      {
        BitStream stream(message.size());
        std::memcpy(stream.GetData(), message.data(), stream.GetByteLength());
        UnpackStream(peerID, stream); // todo: peerid here is stub for now until can get peerid from incoming messages
      }
    }

    incomingMessages.clear();
    
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

                if (!newPacket.HasContents())
                {
                    continue;
                }

                // Add message to connection manager
                BitStream finalPacketStream = newPacket.BuildStream();
                std::vector<unsigned char> message;
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
    m_GhostManager.CreatePeer(peerID);
    // Add a player into the scene
    m_GhostManager.ReplicateGhostObjectsToPeer(peerID);
    Scene::GetInstance().CreateNewPlayer(peerID);
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

    bool didAllSucceed = true;

    didAllSucceed = didAllSucceed && m_MoveManager.WritePacket(packet);
    didAllSucceed = didAllSucceed && m_EventManager.WritePacket(packet, m_PeerTransmissionInfos[peerID].m_TransmissionRecords.back());
    didAllSucceed = didAllSucceed && m_GhostManager.WritePacket(packet, m_PeerTransmissionInfos[peerID].m_TransmissionRecords.back());

    return didAllSucceed;
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
        m_EventManager.ReadStream(sourcePeerID, stream);
        m_EventManager.ProcessEvents();
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
