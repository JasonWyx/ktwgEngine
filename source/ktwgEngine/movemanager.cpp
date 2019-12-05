#include "movemanager.h"
#include "streammanager.h"

MoveManager::MoveManager()
{
}

MoveManager::~MoveManager()
{
}

bool MoveManager::ReadPacket(Packet& packet)
{
    if (StreamManager::GetInstance().IsServer())
    {
        // Something about reading the input once and discarding the rest
    }
    return false;
}

bool MoveManager::WritePacket(Packet& packet)
{
    // Something about putting the moves into 3 packets and sending to the server
    return false;
}

void MoveManager::ResetTransmission()
{
    // Use this if you want to drop transmission
}
