#include "ghostobject.h"
#include "ghostmanager.h"
#include "streammanager.h"

GhostObject::GhostObject(GhostID ghostNetID)
    : m_GhostNetID(ghostNetID)
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

    bool isServer = StreamManager::GetInstance().IsServer();

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();
        if ((authority == Client && !isServer || authority == Server && isServer) &&
            m_GhostProperties[i]->IsPropertyChanged())
        {
            result[i] = true;
        }
    }

    return result;
}

GhostStateMask GhostObject::GetStateMaskAndCheckNeedUpdate(bool& needUpdate)
{
    GhostStateMask result(m_GhostProperties.size());

    bool isServer = StreamManager::GetInstance().IsServer();

    for (size_t i = 0; i < m_GhostProperties.size(); ++i)
    {
        NetAuthority authority = m_GhostProperties[i]->GetAuthority();

        if ((authority == Client && !isServer || authority == Server && isServer) && 
            m_GhostProperties[i]->IsPropertyChanged())
        {
            result[i] = true;
            needUpdate = true;
        }
    }

    return result;
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

    for (size_t i = 0; i < stateMask.size(); ++i)
    {
        if (stateMask[i])
        {
            m_GhostProperties[i]->WriteStream(stream);
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
