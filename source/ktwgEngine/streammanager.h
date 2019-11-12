#pragma once
#include "singleton.h"
#include "connectionmanager.h"
#include "bitstream.h"
#include <memory>
#include <vector>
#include <list>

class GhostTransmissionRecord;
class EventTransmissionRecord;

class StreamManagerInterface
{
    virtual bool ProcessIncomingPacket(BitStream& stream) = 0;
    virtual bool ProcessOutgoingPacket(BitStream& stream) = 0;
    virtual void NotifyPacketStatus(uint32_t packetID, uint32_t packetStatus) = 0;
};

class StreamManager : public Singleton<StreamManager>, public StreamManagerInterface
{
public:     // Public type declarations

    struct TransmissionRecord
    {
        uint32_t            m_PacketID;
        TransmissionRecord* m_NextTransmissionRecord;

        std::list<GhostTransmissionRecord>  m_GhostRecordList;
        std::list<EventTransmissionRecord>  m_EventRecordList; // Only stored guaranteed events && in order of sequence as well
    };

private:    // Private type declarations

    // The local client stream manager. Accepts packets from server and sends updates to server for ghost objects.
    class ClientStreamManager : public StreamManagerInterface
    {
    public:

        virtual bool ProcessIncomingPacket(BitStream& stream) override;
        virtual bool ProcessOutgoingPacket(BitStream& stream) override;

    private:

        // GhostManager
        // EventManager
        // MoveManager
    };

    // The server stream manager. Accepts packets from clients and broadcasts them to other connected peers.
    class ServerStreamManager : public StreamManagerInterface
    {
    private:    // Private type declarations

        struct NetPeerStreamManager
        {
            uint32_t m_NetPeerID;
            
            // GhostManager
            // EventManager
            // MoveManager
        };

    public:

        virtual bool ProcessIncomingPacket(BitStream& stream) override;
        virtual bool ProcessOutgoingPacket(BitStream& stream) override;

    private:

        std::list<std::unique_ptr<NetPeerStreamManager>> m_NetPeerStreamManagers;

    };

public:

    virtual bool ProcessIncomingPacket(BitStream& stream) override;
    virtual bool ProcessOutgoingPacket(BitStream& stream) override;

    void InitializeClient();
    void ShutdownClient();

    void InitializeServer();
    void ShutdownServer();

private:

    std::unique_ptr<ClientStreamManager> m_ClientStreamManager;
    std::unique_ptr<ServerStreamManager> m_ServerStreamManager;

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

};
