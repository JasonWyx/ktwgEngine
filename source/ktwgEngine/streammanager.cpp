#include "streammanager.h"

StreamManager::StreamManager()
{
}

StreamManager::~StreamManager()
{
}

bool StreamManager::SendPacket(Packet& packet)
{
    return false;
}

bool StreamManager::ReceivePacket(Packet& packet)
{
    return false;
}

void StreamManager::NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus)
{
}

void StreamManager::InitializeClient()
{
}

void StreamManager::ShutdownClient()
{
}

void StreamManager::UpdateClient()
{
}

void StreamManager::InitializeServer()
{
}

void StreamManager::ShutdownServer()
{
}

void StreamManager::UpdateServer()
{
}

void StreamManager::InitializeInternal()
{
}

void StreamManager::ShutdownInternal()
{
}
