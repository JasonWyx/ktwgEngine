#include "streammanagerserver.h"
#include "streammanagerclient.h"

StreamManagerServer::StreamManagerServer(StreamManagerClient* streamManagerClient)
    : m_LocalClientStreamManager(streamManagerClient)
{
}

StreamManagerServer::~StreamManagerServer()
{
}

bool StreamManagerServer::ProcessIncomingPacket(Packet& packet)
{
    return false;
}

bool StreamManagerServer::ProcessOutgoingPacket(Packet& packet)
{
    return false;
}

void StreamManagerServer::NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus)
{
    auto iter = m_NetPeerStreamManagers.find(netPeerID);
    if (iter != m_NetPeerStreamManagers.end())
    {
        NetPeerStreamManager* netPeerStreamManager = iter->second;

        for (const TransmissionRecord& transmissionRecord : netPeerStreamManager->m_TransmissionRecords)
        {
            // Jason TODO: find ghost record with matching packet id and append into list of next properties
        }
        // And vice versa for events
    }

}

void StreamManagerServer::AddNetPeerStreamManager(uint32_t netPeerID)
{
    NetPeerStreamManager* netPeerStreamManager = new NetPeerStreamManager();

    auto [entry, isEmplaceSuccess] = m_NetPeerStreamManagers.try_emplace(netPeerID, netPeerStreamManager);

    if (!isEmplaceSuccess)
    {
        // Tried to emplace a new stream manager entry with a net peer id that already exists. Previous manager was not cleaned up properly.
        delete netPeerStreamManager;
    }
}

void StreamManagerServer::RemoveNetPeerStreamManager(uint32_t netPeerID)
{
    auto iter = m_NetPeerStreamManagers.find(netPeerID);
    if (iter != m_NetPeerStreamManagers.end())
    {
        delete iter->second;
        m_NetPeerStreamManagers.erase(iter);
    }
}
