#pragma once
#include "singleton.h"
#include "connectionmanager.h"
#include "ghostmanager.h"
#include "eventmanager.h"
#include "movemanager.h"
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

#ifdef CLIENT
    void InitializeClient();
    void ShutdownClient();
    void UpdateClient();
#else
    void InitializeServer();
    void ShutdownServer();
    void UpdateServer();

	void CreatePeer(PeerID peerID);
	void RemovePeer(PeerID peerID);
#endif

    PeerID GetPeerID() const { return m_PeerID; }
    void SetPeerID(const PeerID& peerID) { m_PeerID = peerID; }

	bool IsServer()
	{
#ifdef CLIENT
		return false;
#else
		return true;
#endif
	}

	GhostManager& GetGhostManager() { return m_GhostManager; }
	EventManager& GetEventManager() { return m_EventManager; }
	MoveManager& GetMoveManager() { return m_MoveManager; }

private:

    virtual void InitializeInternal() override;
    virtual void ShutdownInternal() override;

    Packet CreatePacket();

    PeerID m_PeerID;
    PacketID m_LastPacketID;
    size_t m_FramePacketCount; // This is the number of packets that have been created this frame
    size_t m_MaxFramePacketCount;

	GhostManager m_GhostManager;
	EventManager m_EventManager;
	MoveManager m_MoveManager;

    unsigned m_TimesPackedInTimeFrame[3];
    double m_LoggingTime;

    float m_AveragePacketsPerSecond = 0.0f;

    std::map<PacketID, TransmissionRecord*> m_TransmissionRecordMap;

    struct TransmissionInfo
    {
        std::list<TransmissionRecord> m_TransmissionRecords;
    };

#ifdef CLIENT
    bool PackPacket(Packet& packet);
    TransmissionInfo m_TransmissionInfo;
#else
    bool PackPacket(PeerID peerID, Packet& packet);
    std::map<PeerID, TransmissionInfo> m_PeerTransmissionInfos;
#endif
    void UnpackStream(const PeerID sourcePeerID, BitStream& stream);


};
