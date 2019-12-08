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
    virtual void Sync() = 0; // Sync just copies new into old so that IsPropertyChanged will be correct
    NetAuthority GetAuthority() const { return m_Authority; }

private:

    NetAuthority m_Authority;
};

template<typename T>
class GhostPropertyPOD : public GhostProperty
{
public:

    GhostPropertyPOD(T& property, NetAuthority authority, size_t bitSize = sizeof(T) * 8) 
        : GhostProperty(authority)
        , m_CurrentValue(property)
        , m_PreviousValue()
        , m_BitCount(bitSize)
    { }

    virtual void WriteStream(BitStream& stream) override
    {
        if (m_BitCount != sizeof(T) * 8)
        {
            stream.Write(m_CurrentValue, m_BitCount);
        }
        else
        {
            stream << m_CurrentValue;
        }
    }

    virtual void ReadStream(BitStream& stream) override
    {
        if (m_BitCount != sizeof(T) * 8)
        {
            stream.Read(m_CurrentValue, m_BitCount);
        }
        else
        {
            stream >> m_CurrentValue;
        }
    }

    virtual bool IsPropertyChanged() const override
    {
        return m_CurrentValue != m_PreviousValue;
    }

    virtual void Sync() override
    {
        m_PreviousValue = m_CurrentValue;
    }

    constexpr size_t GetPropertyBitSize() const { return m_BitCount; }


private:

    T& m_CurrentValue;
    T m_PreviousValue;
    const size_t m_BitCount;
};

template<typename T>
class CustomGhostProperty : public GhostProperty
{
public:

  CustomGhostProperty(T& property, NetAuthority authority)
    : GhostProperty(authority)
    , m_CurrentValue(property)
    , m_PreviousValue()
  { }

  virtual void WriteStream(BitStream& stream) override
  {
    stream << m_CurrentValue;
  }

  virtual void ReadStream(BitStream& stream) override
  {
    stream >> m_CurrentValue;
  }

  virtual bool IsPropertyChanged() const override
  {
    return m_CurrentValue != m_PreviousValue;
  }

  virtual void Sync() override
  {
    m_PreviousValue = m_CurrentValue;
  }

private:

  T& m_CurrentValue;
  T m_PreviousValue;
};

BitStream& operator<<(BitStream& stream, const GhostStateMask& stateMask);
BitStream& operator>>(BitStream& stream, GhostStateMask& stateMask);
