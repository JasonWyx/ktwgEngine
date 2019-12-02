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

bool StreamManagerServer::ProcessOutgoingPacket(Packet& packet)
{
    
    return false;
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
