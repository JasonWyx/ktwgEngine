#include "movemanager.h"
#include "streammanager.h"

MoveManager::MoveManager()
{
}

MoveManager::~MoveManager()
{
}

void MoveManager::ReadStream(BitStream& stream)
{
    // Something about reading client input and moving control objects
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
