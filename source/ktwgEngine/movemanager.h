#pragma once
#include "packet.h"
#include "movestate.h"
#include "movecontrolobject.h"
#include <queue>

class MoveManager
{
public:

    MoveManager();
    ~MoveManager();

    void ReadStream(const PeerID peerID, BitStream& stream);
    bool WritePacket(Packet& packet);
    void DropData();

#ifdef CLIENT
    void PushMoveState(const MoveState& command);
#else
    void RegisterMoveObject(PeerID peerID, MoveControlObject* moveControlObject);
    void UnregisterMoveObject(PeerID peerID);
#endif

private:

    struct MoveStateObject
    {
        MoveControlObject* m_MoveControlObject;
        MoveState m_MoveStateCache;
        unsigned m_PacketCount = 3;
        unsigned m_MoveSequenceNumber = 0;
    };

#ifdef CLIENT
    std::queue<MoveState> m_MoveStateBuffer;
    MoveStateObject m_MoveStateObject;
    unsigned m_MoveSequenceNumber = 0;
#else
    std::map<PeerID, MoveStateObject> m_MoveStateObject;
#endif

    unsigned m_TimesPacked = 0;
    
public:

    float m_AverageTimesPacked = 0.0f;

    void PrintLog();
};