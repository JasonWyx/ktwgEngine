#include "streammanager.h"

bool StreamManager::ProcessIncomingPacket(BitStream& stream)
{
    return false;
}

bool StreamManager::ProcessOutgoingPacket(BitStream& stream)
{
    return false;
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

bool StreamManager::ClientStreamManager::ProcessIncomingPacket(BitStream& stream)
{
    return false;
}

bool StreamManager::ClientStreamManager::ProcessOutgoingPacket(BitStream& stream)
{
    return false;
}

bool StreamManager::ServerStreamManager::ProcessIncomingPacket(BitStream& stream)
{
    return false;
}

bool StreamManager::ServerStreamManager::ProcessOutgoingPacket(BitStream& stream)
{
    return false;
}
