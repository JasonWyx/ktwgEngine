#pragma once
#include "singleton.h"
#include "connectionmanager.h"
#include "streammanagerclient.h"
#include "streammanagerserver.h"
#include "ghostmanager.h"
#include "eventmanager.h"
#include "bitstream.h"
#include <memory>
#include <vector>
#include <list>
#include <map>

class StreamManager : public Singleton<StreamManager>
{
public:

    StreamManager();
    ~StreamManager();

    bool ProcessIncomingPacket(Packet& packet);
    bool ProcessOutgoingPacket(Packet& packet);
    void NotifyPacketStatus(PeerID fromPeerID, PacketID packetID, PacketStatus packetStatus);

    // Client Functions
    void InitializeClient();
    void ShutdownClient();
    StreamManagerClient* GetClient() { return m_ClientStreamManager; }
    
    // Server Functions
    void InitializeServer();
    void ShutdownServer();
    StreamManagerServer* GetServer() { return m_ServerStreamManager; }

    bool IsServer() { return m_ServerStreamManager != nullptr; }

private:

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

    StreamManagerClient* m_ClientStreamManager;
    StreamManagerServer* m_ServerStreamManager;

    std::vector<TransmissionRecord> m_TransmissionRecords;

};
