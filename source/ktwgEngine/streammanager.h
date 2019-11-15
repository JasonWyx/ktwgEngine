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

class GhostTransmissionRecord
{

};

class EventTransmissionRecord
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

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;

    void InitializeClient();
    void InitializeServer();
    void ShutdownClient();
    void ShutdownServer();

private:

    std::unique_ptr<StreamManagerClient> m_ClientStreamManager;
    std::unique_ptr<StreamManagerServer> m_ServerStreamManager;

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

};
