#pragma once
#include <stdint.h>
#include "ghostproperty.h"
#include "ghostobjectids.h"
#include <vector>
#include <type_traits>

using GhostNetID = unsigned int;

class GhostObject
{
public:

    GhostObject(uint32_t ghostNetID);
    virtual ~GhostObject();
    
    // Jason: @Aaron you might want to change this
    void WriteStream(BitStream& stream, const std::vector<bool>& propertyFlags);
    void ReadStream(BitStream& stream, const std::vector<bool>& propertyFlags);

    template<typename T>
    void RegisterGhostProperty(T& property)
    {
        m_Properties.push_back(MakeGhostProperty(property, m_Properties.size()));
    }

private:

    GhostNetID m_GhostNetID; // Jason: This is the ID that should be the same for all peers. Server will assign this.
    GhostClassID m_GhostClassID; // Jason: This COULD be used to identify the class in the packet
    std::vector<IGhostProperty*> m_Properties;
};