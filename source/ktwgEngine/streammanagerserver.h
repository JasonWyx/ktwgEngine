#pragma once
#include "istreammanager.h"
#include <list>
#include <memory>

struct NetPeerStreamManager
{
    uint32_t m_NetPeerID;

    // GhostManager
    // EventManager
    // MoveManager
};

// The server stream manager. Accepts packets from clients and broadcasts them to other connected peers.
class StreamManagerServer : public IStreamManager
{
public:

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;

private:

    std::list<std::unique_ptr<NetPeerStreamManager>> m_NetPeerStreamManagers;

};