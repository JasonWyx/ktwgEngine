#pragma once
#include "singleton.h"
#include "connectionmanager.h"
#include "istreammanager.h"
#include "streammanagerclient.h"
#include "streammanagerserver.h"
#include "bitstream.h"
#include <memory>
#include <vector>
#include <list>

struct GhostTransmissionRecord
{

};

struct EventTransmissionRecord
{

};

struct TransmissionRecord
{
    uint32_t            m_PacketID;
    TransmissionRecord* m_NextTransmissionRecord;

    std::list<GhostTransmissionRecord>  m_GhostRecordList;
    std::list<EventTransmissionRecord>  m_EventRecordList; // Only stored guaranteed events && in order of sequence as well
};

class StreamManager : public Singleton<StreamManager>, public IStreamManager
{
public:

    StreamManager();
    virtual ~StreamManager();

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;
    virtual void NotifyPacketStatus(NetPeerID netPeerID, PacketID packetID, PacketStatus packetStatus) override;

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

    StreamManagerClient* m_ClientStreamManager;
    StreamManagerServer* m_ServerStreamManager;

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

};
