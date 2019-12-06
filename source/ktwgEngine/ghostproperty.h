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
    virtual ~GhostProperty() = default;

    virtual void WriteStream(BitStream& stream) = 0;
    virtual void ReadStream(BitStream& stream) = 0;
    virtual bool IsPropertyChanged() const = 0;
    virtual void SyncValues() = 0;
    NetAuthority GetAuthority() const { return m_Authority; }

private:

    NetAuthority m_Authority;
};

template<typename T>
class GhostPropertyVirtual : public GhostProperty
{
public:

    GhostPropertyVirtual(T& property, NetAuthority authority, size_t bitSize = sizeof(T) * 8) 
        : GhostProperty(authority)
        , m_ValueRef(property)
        , m_OldValue()
        , m_BitCount(bitSize)
    { }

    virtual void WriteStream(BitStream& stream) override
    {
        if (m_BitCount != sizeof(T) * 8)
        {
            stream.Write(m_ValueRef, m_BitCount);
        }
        else
        {
            stream << m_ValueRef;
        }
    }

    virtual void ReadStream(BitStream& stream) override
    {
        if (m_BitCount != sizeof(T) * 8)
        {
            stream.Read(m_ValueRef, m_BitCount);
        }
        else
        {
            stream >> m_ValueRef;
        }
    }

    virtual bool IsPropertyChanged() const override
    {
        return m_ValueRef != m_OldValue;
    }

    virtual void SyncValues() override
    {
        m_OldValue = m_ValueRef;
    }

    constexpr size_t GetPropertyBitSize() const { return m_BitCount; }


private:

    T& m_ValueRef;
    T m_OldValue;
    const size_t m_BitCount;
};

BitStream& operator<<(BitStream& stream, const GhostStateMask& stateMask);
BitStream& operator>>(BitStream& stream, GhostStateMask& stateMask);
