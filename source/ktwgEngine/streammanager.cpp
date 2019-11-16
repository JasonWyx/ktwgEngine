#include "streammanager.h"

StreamManager::StreamManager()
{
}

StreamManager::~StreamManager()
{
}

bool StreamManager::ProcessIncomingPacket(BitStream& stream)
{
    return false;
}

bool StreamManager::ProcessOutgoingPacket(BitStream& stream)
{
    return false;
}

void StreamManager::NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus)
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
