#include "streammanagerclient.h"

StreamManagerClient::StreamManagerClient()
{
}

StreamManagerClient::~StreamManagerClient()
{
}

bool StreamManagerClient::ProcessIncomingPacket(Packet& stream)
{
    return false;
}

bool StreamManagerClient::ProcessOutgoingPacket(Packet& stream)
{
    return false;
}

void StreamManagerClient::NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus)
{
}
