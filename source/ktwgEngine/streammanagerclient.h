#pragma once
#include "istreammanager.h"

// The local client stream manager. Accepts packets from server and sends updates to server for ghost objects.
class StreamManagerClient : public IStreamManager
{
public:

    StreamManagerClient();
    ~StreamManagerClient();

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;

private:

    std::uint32_t m_NetPeerID;

    // GhostManager
    // EventManager
    // MoveManager
};