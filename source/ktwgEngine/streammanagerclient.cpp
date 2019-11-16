#include "streammanagerclient.h"

StreamManagerClient::StreamManagerClient()
{
}

StreamManagerClient::~StreamManagerClient()
{
}

bool StreamManagerClient::ProcessIncomingPacket(BitStream& stream)
{
    return false;
}

bool StreamManagerClient::ProcessOutgoingPacket(BitStream& stream)
{
    return false;
}

void StreamManagerClient::NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus)
{
}
