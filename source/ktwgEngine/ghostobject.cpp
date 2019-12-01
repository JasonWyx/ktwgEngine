#include "ghostobject.h"
#include "ghostmanager.h"
#include "streammanager.h"
#include <cassert>

GhostObject::GhostObject(GhostID ghostNetID)
    : m_GhostNetID(ghostNetID)
    , m_LatestGhostTransmissionRecord(nullptr)
    , m_GhostProperties()
{
    StreamManager::GetInstance().GetClient()->GetGhostManager().RegisterGhostObject(this);
}

GhostObject::~GhostObject()
{
    StreamManager::GetInstance().GetClient()->GetGhostManager().UnregisterGhostObject(this);

    for (GhostProperty* property : m_GhostProperties)
    {
        delete property;
    }
}

GhostStateMask GhostObject::GetStateMask() const
{
    GhostStateMask result(m_GhostProperties.size());

    for (size_t i = 0; i < m_RetransmissionMask.size(); ++i)
    {
        result[i] = m_RetransmissionMask[i];
    }
    
    bool isServer = StreamManager::GetInstance().IsServer();

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();
        if ((authority == NetAuthority::Client && !isServer || authority == NetAuthority::Server && isServer) && m_GhostProperties[i]->IsPropertyChanged())
        {
            result[i] = true;
        }
    }

    return result;
}

GhostStateMask GhostObject::GetStateMaskAndCheckNeedUpdate(bool& needUpdate)
{
    GhostStateMask result = m_RetransmissionMask;
    bool isServer = StreamManager::GetInstance().IsServer();
                 
    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();

        if (result[i] == true)
        {
            needUpdate = true;
        }
        else if ((authority == NetAuthority::Client && !isServer || authority == NetAuthority::Server && isServer) && m_GhostProperties[i]->IsPropertyChanged())
        {
            result[i] = true;
            needUpdate = true;
        }
    }

    return result;
}

void GhostObject::SetRetransmissionMask(const GhostStateMask& stateMask)
{
    assert(m_GhostProperties.size() == stateMask.size(), "GhostObject::SetTransmissionMask: The size of the state mask should the same as the number of ghost properties!");  
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
    stream << m_GhostNetID;

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
}
