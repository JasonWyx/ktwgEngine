#pragma once
#include "bitstream.h"

using GhostStateMask = std::vector<bool>;

enum class NetAuthority
{
    Client,
    Server
};

class GhostProperty
{
public:
    explicit GhostProperty(NetAuthority authority) : m_Authority(authority) { }
    virtual ~GhostProperty() { }

    virtual void WriteStream(BitStream& stream) = 0;
    virtual void ReadStream(BitStream& stream) = 0;
    virtual bool IsPropertyChanged() const = 0;
    NetAuthority GetAuthority() const { return m_Authority; }

private:

    const NetAuthority m_Authority; // Indicates who should own this variable
};

template<typename T>
class GhostPropertyVirtual : public GhostProperty
{
public:

    GhostPropertyVirtual(T& property, NetAuthority authority = NetAuthority::Client, size_t bitSize = sizeof(T) * 8) 
        : GhostProperty(authority)
        , m_ValueRef(property)
        , m_PreviousValue()
        , m_NumBits(bitSize)
    { }

    void WriteStream(BitStream& stream) override
    {
        if (m_NumBits != sizeof(T) * 8)
        {
            stream.Write(m_ValueRef, m_NumBits);
        }
        else
        {
            stream << m_ValueRef;
        }
        m_PreviousValue = m_ValueRef;
    }

    void ReadStream(BitStream& stream) override
    {
        if (m_NumBits != sizeof(T) * 8)
        {
            stream.Read(m_ValueRef, m_NumBits);
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

    constexpr size_t GetPropertyBitSize() const { return m_NumBits; }

private:

    T& m_ValueRef;
    T m_PreviousValue;
    const size_t m_NumBits;
};

BitStream& operator<<(BitStream& stream, const GhostStateMask& stateMask);
BitStream& operator>>(BitStream& stream, GhostStateMask& stateMask);
