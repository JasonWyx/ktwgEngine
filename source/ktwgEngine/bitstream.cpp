#include "bitstream.h"

BitStream::BitStream() : BitStream(0)
{
}

BitStream::BitStream(size_t bytes)
    : m_Buffer(bytes)
    , m_BitPosition(0)
{
    
}

BitStream::BitStream(const uint8_t* data, size_t length) : BitStream(length)
{
    std::memcpy(m_Buffer.data(), data, length);
}

BitStream::BitStream(const std::vector<uint8_t>& data)
    : m_Buffer(data)
    , m_BitPosition(0)
{
}

BitStream::BitStream(std::vector<uint8_t>&& data)
    : m_Buffer(std::move(data))
    , m_BitPosition(0)
{
}

void BitStream::Clear()
{
    m_Buffer.clear();
    m_BitPosition = 0;
}

BitStream BitStream::operator+(const BitStream& rhs) const
{
    return BitStream(*this) += rhs;
}

BitStream& BitStream::operator+=(const BitStream& rhs)
{
    const size_t shiftRight = m_BitPosition % ByteToBits;
    const size_t shiftLeft = ByteToBits - shiftRight;

    if (!rhs.m_Buffer.empty())
    {
        std::vector<uint8_t> tmp = rhs.m_Buffer;

        if (!m_Buffer.empty())
        {
            m_Buffer.back() |= tmp.front() >> shiftLeft;
        }

        for (size_t i = 0; i < tmp.size(); i++)
        {
            if (i + 1 == tmp.size())
            {
                tmp[i] <<= shiftRight;
            }
            else
            {
                tmp[i] = tmp[i] << shiftRight | tmp[i + 1] >> shiftLeft;
            }
        }

        m_Buffer.insert(m_Buffer.end(), tmp.begin(), tmp.end());
    }

    return *this;
}
