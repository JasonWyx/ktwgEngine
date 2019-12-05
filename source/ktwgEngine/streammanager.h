#pragma once
#include "singleton.h"
#include "connectionmanager.h"
#include "streammanagerclient.h"
#include "streammanagerserver.h"
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

    void Update();

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
    PeerID GetPeerID() const { return m_PeerID; }
    void SetPeerID(const PeerID& peerID) { m_PeerID = peerID; }

private:

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

    bool SendPacket(Packet& packet);
    bool ReceivePacket(Packet& packet);
    void NotifyPacketStatus(PacketID packetID, PacketStatus packetStatus);
    
    Packet CreatePacket();

    StreamManagerClient* m_ClientStreamManager;
    StreamManagerServer* m_ServerStreamManager;

    PeerID m_PeerID;
    PacketID m_LastPacketID;
    size_t m_FramePacketCount; // This is the number of packets that have been created this frame
    size_t m_MaxFramePacketCount;
};
