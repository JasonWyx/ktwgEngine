#pragma once
#include "bitstream.h"

using GhostPropertyIndex = unsigned int;
using GhostStateMask = std::vector<bool>;


enum NetAuthority
{
    Client,
    Server
};


class GhostProperty
{
public:
    GhostProperty(NetAuthority authority) : m_Authority(authority) { }
    virtual ~GhostProperty() { }

    virtual bool WriteStream(BitStream& stream) = 0;
    virtual void ReadStream(BitStream& stream) = 0;
    virtual bool IsPropertyChanged() const = 0;
    NetAuthority GetAuthority() const { return m_Authority; }

private:

    NetAuthority m_Authority; // Indicates who should own this variable
};

template<typename T, size_t N = sizeof(T) * 8>
class GhostPropertyVirtual : public GhostProperty
{
public:

    GhostPropertyVirtual(T& property, NetAuthority authority = NetAuthority::Client) 
        : GhostProperty(authority)
        , m_ValueRef(property)
        , m_PreviousValue()
    { }

    void WriteStream(BitStream& stream) override
    {
        if constexpr (N != sizeof(T) * 8)
        {
            stream.Write(m_ValueRef, N);
        }
        else
        {
            stream << m_ValueRef;
        }
        m_PreviousValue = m_ValueRef;
    }

    void ReadStream(BitStream& stream) override
    {
        if constexpr (N != sizeof(T) * 8)
        {
            stream.Read(m_ValueRef, N);
        }
        else
        {
            stream >> m_ValueRef;
        }

        m_PreviousValue = m_ValueRef;
    }

    bool IsPropertyChanged() const override
    {
        return m_ValueRef != m_PreviousValue;
    }

    constexpr size_t GetPropertyBitSize() const { return N; }

private:

    T& m_ValueRef;
    T m_PreviousValue;
};

template<typename T, size_t N = sizeof(T) * 8>
GhostPropertyVirtual<T, N>* MakeGhostProperty(T& property)
{
    // Jason: Cannot handle reference to const for now / Should not be const anyway if it needs to be updated by the network
    static_assert(!std::is_const_v<T>);

    return new GhostPropertyVirtual<T, N>(property);
}

BitStream& operator<<(BitStream& stream, const GhostStateMask& stateMask);
BitStream& operator>>(BitStream& stream, GhostStateMask& stateMask);
