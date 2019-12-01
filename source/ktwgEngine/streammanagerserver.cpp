#include "streammanagerserver.h"
#include "streammanagerclient.h"
#include <algorithm>

StreamManagerServer::StreamManagerServer(StreamManagerClient* streamManagerClient)
    : m_LocalClientStreamManager(streamManagerClient)
{
}

StreamManagerServer::~StreamManagerServer()
{

}

bool StreamManagerServer::ProcessOutgoingPackets(std::vector<std::pair<PeerID, Packet>>& packets)
{
    // Process local transmission first
    BitStream localBitStream;
    TransmissionRecord localTransmissionRecord;

    m_LocalClientStreamManager->GetGhostManager().WriteStream(localBitStream, localTransmissionRecord);
    //m_LocalClientStreamManager->GetEventManager().WritePacketBitStream(localBitStream, localTransmissionRecord);

    // Fill all packets
    for (auto& [peerID, packet] : packets)
    {
        // Update transmission record of peer
        std::vector<TransmissionRecord>& peerTransmissionRecords = m_PeerTransmissionRecords[peerID];
        peerTransmissionRecords.push_back(localTransmissionRecord);
        peerTransmissionRecords.back().m_PacketID = packet.m_ID;
    }

    return true;
}

void StreamManagerServer::NotifyPacketStatus(PeerID peerID, PacketID packetID, PacketStatus packetStatus)
{
    auto iter = m_PeerTransmissionRecords.find(peerID);
    if (iter != m_PeerTransmissionRecords.end())
    {
        if (packetStatus == PacketStatus::Success)
        {
            iter->second.erase(std::remove_if(iter->second.begin(), iter->second.end(),
                [&packetID](const TransmissionRecord& record)
                {
                    return record.m_PacketID == packetID;
                }
            ));
        }

        for (const TransmissionRecord& transmissionRecord : iter->second)
        {
            // Jason TODO: find ghost record with matching packet id and append into list of next properties
            // Need to make sure retransmission takes into account the packets in flight also
            
        }
        // And vice versa for events
    }

}

void StreamManagerServer::CreatePeer(PeerID peerID)
{
    auto iter = m_PeerTransmissionRecords.find(peerID);
    if (iter != m_PeerTransmissionRecords.end())
    {
        _ASSERT(false); // Trying to create a peer with an existing peer ID!
    }

    // Implicitly create new peer entry
    m_PeerTransmissionRecords[peerID].clear();
}

void StreamManagerServer::RemovePeer(PeerID peerID)
{
    auto iter = m_PeerTransmissionRecords.find(peerID);
    if (iter != m_PeerTransmissionRecords.end())
    {
        m_PeerTransmissionRecords.erase(iter);
    }
}
