#include "movemanager.h"
#include "streammanager.h"

MoveManager::MoveManager()
{
}

MoveManager::~MoveManager()
{
}

void MoveManager::ReadStream(const PeerID sourcePeerID, BitStream& stream)
{
#ifdef CLIENT

#else
    // Something about reading client input and moving control objects
    unsigned moveSequenceNumber;
    stream >> moveSequenceNumber;

    // Only update the move state if the sequence number has changed
    if (moveSequenceNumber >= m_MoveStateObject[sourcePeerID].m_MoveSequenceNumber)
    {
        for (size_t i = 0; i < MoveStateFlags::Count; i++)
        {
            stream >> m_MoveStateObject[sourcePeerID].m_MoveControlObject->m_MoveState[i];
        }

        // Bump the number over the over flow
        if (moveSequenceNumber == 0xFFFFFFFF)
        {
            moveSequenceNumber = 0;
        }
        m_MoveStateObject[sourcePeerID].m_MoveSequenceNumber = moveSequenceNumber;
    }
    else
    {
        bool dummy;
        stream >> dummy >> dummy >> dummy >> dummy;
    }


#endif
}

bool MoveManager::WritePacket(Packet& packet)
{
#ifdef CLIENT
    // Packet has been sent 3 times and move buffer not empty
    if (m_MoveStateObject.m_PacketCount >= 3)
    {
        if (!m_MoveStateBuffer.empty())
        {
            m_MoveStateObject.m_MoveStateCache = m_MoveStateBuffer.front();
            m_MoveStateObject.m_PacketCount = 0;
            m_MoveSequenceNumber++;
            m_MoveStateBuffer.pop();
        }
        else
        {
            // No pending moves, successfully packed nothing
            return true;
        }
    }

    // Pack state into stream
    packet.m_MoveStream << m_MoveSequenceNumber;
    for (size_t i = 0; i < MoveStateFlags::Count; i++)
    {
        packet.m_MoveStream << m_MoveStateObject.m_MoveStateCache[i];
    }
    m_TimesPacked++;
    m_MoveStateObject.m_PacketCount++;
#endif
    return true;
}

void MoveManager::DropData()
{
    // Use this if you want to drop transmission
#ifdef CLIENT
    m_MoveStateObject.m_PacketCount = 0;
#endif
}

#ifdef CLIENT

void MoveManager::PushMoveState(const MoveState& moveState)
{
    // Don't need to dupliate moves
    if (m_MoveStateBuffer.empty() || m_MoveStateBuffer.back() != moveState)
    {
        m_MoveStateBuffer.push(moveState);
    }
}

#else

void MoveManager::RegisterMoveObject(PeerID peerID, MoveControlObject* moveControlObject)
{
    m_MoveStateObject[peerID].m_MoveControlObject = moveControlObject;
}

void MoveManager::UnregisterMoveObject(PeerID peerID)
{
    m_MoveStateObject.erase(peerID);
}

#endif

void MoveManager::PrintLog()
{
    std::cout << "    Times Packed : " << m_TimesPacked << std::endl;

    m_AverageTimesPacked = (m_AverageTimesPacked + m_TimesPacked) / 2;

    m_TimesPacked = 0;
}
