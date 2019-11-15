#pragma once
#include <stdint.h>
#include "bitstream.h"
#include "ghostobjectids.h"
#include <vector>
#include <type_traits>

enum NetAuthority
{
    Client,
    Server
};

class IGhostProperty
{
public:
    IGhostProperty() = default;
    virtual ~IGhostProperty() { }
    virtual void WriteStream(BitStream& stream) = 0;
    virtual void ReadStream(BitStream& stream) = 0;
};

template<typename T>
class GhostProperty : public IGhostProperty
{
public:
    
    GhostProperty(T& property, uint32_t propertyIndex) 
        : m_Property(property)
        , m_PropertyIndex(propertyIndex)
    { }

    void WriteStream(BitStream&& stream) override 
    { 
        stream << m_Property; 
    }

    void ReadStream(BitStream&& stream) override 
    { 
        stream >> m_Property; 
    }

    constexpr size_t GetSize() const { return sizeof(T); }
    uint32_t GetPropertyIndex() const { return m_PropertyIndex; }

private:

    T& m_Property;
    uint32_t m_PropertyIndex;
    NetAuthority m_Authority; // Indicates who should own this variable

};



template<typename T>
GhostProperty<T>* MakeGhostProperty(T& property, uint32_t propertyIndex)
{
    // Jason: Cannot handle reference to const for now
    static_assert(!std::is_const_v<T>);

    return new GhostProperty<T>(property, propertyIndex);
}

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

    uint32_t m_GhostNetID; // Jason: This is the ID that should be the same for all peers. Server will assign this.
    GhostClassID m_GhostClassID; // Jason: This COULD be used to idenify the class in the packet
    std::vector<IGhostProperty*> m_Properties;
};