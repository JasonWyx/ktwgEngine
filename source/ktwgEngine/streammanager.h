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

    bool SendPacket(Packet& packet);
    bool ReceivePacket(Packet& packet);
    void NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus);

    // Client Functions
    void InitializeClient();
    void ShutdownClient();
    void UpdateClient();
    StreamManagerClient* GetClient() { return m_ClientStreamManager; }
    
    // Server Functions
    void InitializeServer();
    void ShutdownServer();
    void UpdateServer();
    StreamManagerServer* GetServer() { return m_ServerStreamManager; }

    bool IsServer() { return m_ServerStreamManager != nullptr; }

private:

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

    StreamManagerClient* m_ClientStreamManager;
    StreamManagerServer* m_ServerStreamManager;
};
