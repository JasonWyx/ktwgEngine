#include "ghostobject.h"
#include "ghostmanager.h"
#include "ghosttransmissionrecord.h"
#include "streammanager.h"
#include "entity.h"
#include <cassert>

GhostObject::GhostObject()
    : m_GhostID(0)
    , m_PeerID(0)
    //, m_LatestGhostTransmissionRecord(nullptr)
    , m_GhostProperties()
{
    StreamManager::GetInstance().GetGhostManager().RegisterGhostObject(this);
}

GhostObject::~GhostObject()
{
    StreamManager::GetInstance().GetGhostManager().UnregisterGhostObject(this);

    for (GhostProperty* property : m_GhostProperties)
    {
        delete property;
    }
}

GhostStateMask GhostObject::GetFullStateMask() const
{
    GhostStateMask result(m_GhostProperties.size());

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        const NetAuthority authority = m_GhostProperties[i]->GetAuthority();

#ifdef CLIENT
        if (authority == NetAuthority::Client && m_PeerID == StreamManager::GetInstance().GetPeerID())
        {
            result[i] = true;
        }
#else
        if (authority == NetAuthority::Server)
        {
            result[i] = true;
        }
#endif
    }

    return result;
}

#ifdef CLIENT

GhostStateMask GhostObject::GetStateMaskAndCheckNeedUpdate(bool& outNeedUpdate)
{
    m_StatesToRetransmit.resize(m_GhostProperties.size());

    GhostStateMask result = m_StatesToRetransmit;

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        const NetAuthority authority = m_GhostProperties[i]->GetAuthority();

        if (result[i] == true)
        {
            outNeedUpdate = true;
            continue;
        }

        if (authority == NetAuthority::Client && m_GhostProperties[i]->IsPropertyChanged() && m_PeerID == StreamManager::GetInstance().GetPeerID())
        {
            result[i] = true;
        }
    }

    return result;
}

void GhostObject::SetRetransmissionMask(const GhostStateMask& stateMask)
{
    // The size of the state mask should the same as the number of ghost properties!
    assert(m_GhostProperties.size() == stateMask.size());

    m_StatesToRetransmit.resize(m_GhostProperties.size());
    m_StatesToRetransmit = stateMask;
}

void GhostObject::WriteStream(BitStream& stream, const GhostStateMask& stateMask)
{
    assert(m_GhostProperties.size() == stateMask.size());

    m_StatesToRetransmit.resize(m_GhostProperties.size());

    for (size_t i = 0; i < stateMask.size(); ++i)
    {
        if (stateMask[i])
        {
            m_GhostProperties[i]->WriteStream(stream);
            m_GhostProperties[i]->Sync(); // We sync right after in client mode because we do not need to care about other peers
            m_StatesToRetransmit[i] = false;
        }
    }
}
#else //Server

GhostStateMask GhostObject::GetStateMaskAndCheckNeedUpdate(const PeerID targetPeerID, bool & outNeedUpdate)
{
    m_StatesToBroadcast.resize(m_GhostProperties.size());
    m_StatesToRetransmit[targetPeerID].resize(m_GhostProperties.size());

    GhostStateMask result(m_GhostProperties.size());
    
    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = m_StatesToRetransmit[targetPeerID][i] && m_StatesToBroadcast[i];
    }

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();

        // Already true from broadcast or retransmission, continue
        if (result[i] == true)
        {
            outNeedUpdate = true;
            continue;
        }

        if (authority == NetAuthority::Server && m_GhostProperties[i]->IsPropertyChanged())
        {
            outNeedUpdate = true;
            result[i] = true;
        }
    }

    return result;
}

void GhostObject::SetRetransmissionMask(const PeerID targetPeerID, const GhostStateMask & stateMask)
{
    // The size of the state mask should the same as the number of ghost properties!
    assert(m_GhostProperties.size() == stateMask.size());

    m_StatesToRetransmit[targetPeerID].resize(m_GhostProperties.size());
    m_StatesToRetransmit[targetPeerID] = stateMask;
}

void GhostObject::WriteStream(const PeerID targetPeerID, BitStream & stream, const GhostStateMask & stateMask)
{
    assert(m_GhostProperties.size() == stateMask.size());

    m_StatesToRetransmit[targetPeerID].resize(m_GhostProperties.size());

    for (size_t i = 0; i < stateMask.size(); ++i)
    {
        if (stateMask[i])
        {
            m_GhostProperties[i]->WriteStream(stream);
            m_StatesToRetransmit[targetPeerID][i] = false;
        }
    }
}
#endif

#if CLIENT
void GhostObject::ReplicateGhostObjectToBitstream(BitStream & bitstream)
{
  m_Owner->ReplicateGhostObjectToBitstream(bitstream);
}
#else
void GhostObject::ReplicateGhostObjectToBitstream(const PeerID targetPeerID, BitStream & bitstream)
{
  m_Owner->ReplicateGhostObjectToBitstream(targetPeerID, bitstream);
}
#endif

void GhostObject::ReadStream(BitStream& stream, const GhostStateMask& stateMask)
{
    for (size_t i = 0; i < stateMask.size(); ++i)
    { 
        if (stateMask[i])
        {
            m_GhostProperties[i]->ReadStream(stream);
        }
    }

#ifndef CLIENT
    m_StatesToBroadcast = stateMask;
#endif
}

void GhostObject::SyncProperties()
{
    for (GhostProperty* ghostProperty : m_GhostProperties)
    {
        NetAuthority authority = ghostProperty->GetAuthority();

#ifdef CLIENT
        if (authority == NetAuthority::Client && m_PeerID == StreamManager::GetInstance().GetPeerID())
#else
        if (authority == NetAuthority::Server)
#endif
        {
            ghostProperty->Sync();
        }
    }
}


inline bool GhostObject::IsOwner()
{
    return StreamManager::GetInstance().GetPeerID();
}
