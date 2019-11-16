#pragma once
#include "ighostproperty.h"

using GhostPropertyIndex = unsigned int;

enum NetAuthority
{
    Client,
    Server
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
    GhostPropertyIndex m_PropertyIndex;
    NetAuthority m_Authority; // Indicates who should own this variable

};

template<typename T>
GhostProperty<T>* MakeGhostProperty(T& property, uint32_t propertyIndex)
{
    // Jason: Cannot handle reference to const for now / Should not be const anyway if it needs to be updated by the network
    static_assert(!std::is_const_v<T>);

    return new GhostProperty<T>(property, propertyIndex);
}