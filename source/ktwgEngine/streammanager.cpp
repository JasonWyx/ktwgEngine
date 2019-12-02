#include "streammanager.h"

StreamManager::StreamManager()
{
}

StreamManager::~StreamManager()
{
}

bool StreamManager::ProcessIncomingPacket(Packet& packet)
{
    return false;
}

bool StreamManager::ProcessOutgoingPacket(Packet& packet)
{
    return false;
}

void StreamManager::NotifyPacketStatus(PeerID fromPeerID, PacketID packetID, PacketStatus packetStatus)
{
}

void StreamManager::InitializeClient()
{
}

void StreamManager::ShutdownClient()
{
}

void StreamManager::InitializeServer()
{
}

void StreamManager::ShutdownServer()
{
}

void StreamManager::InitializeInternal()
{
}

void StreamManager::ShutdownInternal()
{
}
