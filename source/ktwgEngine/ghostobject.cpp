#include "ghostobject.h"
#include "ghostmanager.h"
#include "streammanager.h"
#include <cassert>

GhostObject::GhostObject(GhostID ghostID, PeerID owner)
    : m_GhostID(ghostID)
    , m_Owner(owner)
    , m_LatestGhostTransmissionRecord(nullptr)
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

GhostStateMask GhostObject::GetStateMask() const
{
    GhostStateMask result = m_RetransmissionMask;

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();

        if (result[i] == true)
        {
            continue;
        }

#ifdef CLIENT
        if (m_GhostProperties[i]->IsPropertyChanged() && m_Owner == StreamManager::GetInstance().GetPeerID())
#else
        if (m_GhostProperties[i]->IsPropertyChanged())
#endif
        {
            result[i] = true;
        }
    }

    return result;
}

GhostStateMask GhostObject::GetStateMaskAndCheckNeedUpdate(bool& needUpdate)
{
    GhostStateMask result = m_RetransmissionMask;

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();

        if (result[i] == true)
        {
            needUpdate = true;
            continue;
        }

#ifdef CLIENT
        if (m_GhostProperties[i]->IsPropertyChanged() && m_Owner == StreamManager::GetInstance().GetPeerID())
#else
        if (m_GhostProperties[i]->IsPropertyChanged())
#endif
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
    m_RetransmissionMask = stateMask;
}

bool GhostObject::NeedUpdate() const
{
    for (GhostProperty* property : m_GhostProperties)
    {
        if (property->IsPropertyChanged())
        {
            return true;
        }
    }
    return false;
}

void GhostObject::WriteStream(BitStream& stream, const GhostStateMask& stateMask)
{
    if (m_RetransmissionMask.size() != stateMask.size())
    {
        m_RetransmissionMask.resize(stateMask.size());
    }

    for (size_t i = 0; i < stateMask.size(); ++i)
    {
        if (stateMask[i])
        {
            m_GhostProperties[i]->WriteStream(stream);

            // Reset retransmission mask to false
            m_RetransmissionMask[i] = false;
        }
    }
}

void GhostObject::ReadStream(BitStream& stream, const GhostStateMask& stateMask)
{
    for (size_t i = 0; i < stateMask.size(); ++i)
    { 
        if (stateMask[i])
        {
            m_GhostProperties[i]->ReadStream(stream);
        }
    }

    if (StreamManager::GetInstance().IsServer())
    {
        m_ServerTransmissionMask = stateMask;
    }
}

inline bool GhostObject::IsOwner()
{
    return StreamManager::GetInstance().GetPeerID();
}
