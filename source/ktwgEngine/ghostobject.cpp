#include "ghostobject.h"

GhostObject::GhostObject(uint32_t ghostNetID)
    : m_GhostNetID(ghostNetID)
{
    // Maybe register to ghost manager here?
}

GhostObject::~GhostObject()
{
    // Maybe unsubscribe to ghost manager here?
}

void GhostObject::WriteStream(BitStream& stream, const std::vector<bool>& propertyFlags)
{
    for (size_t i = 0; i < propertyFlags.size(); ++i)
    {
        if (propertyFlags[i])
        {
            m_Properties[i]->WriteStream(stream);
        }
    }
}

void GhostObject::ReadStream(BitStream& stream, const std::vector<bool>& propertyFlags)
{
    for (size_t i = 0; i < propertyFlags.size(); ++i)
    {
        if (propertyFlags[i])
        {
            m_Properties[i]->ReadStream(stream);
        }
    }
}
