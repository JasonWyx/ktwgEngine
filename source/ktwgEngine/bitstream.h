#pragma once
#include <type_traits>
#include <cstdlib>
#include <vector>

/**************************************************************************************************/
/*!
    This BitStream class helps to pack bits into compact forms.

    Data Layout:

    [0][1][2]...[N-1]
*/
/**************************************************************************************************/
class BitStream
{
public:

    BitStream();
    BitStream(size_t bytes);
    ~BitStream() = default;

    // Declare default copy/move constructors/assignments
    BitStream(const BitStream&) = default;
    BitStream(BitStream&&) = default;    
    BitStream& operator=(const BitStream&) = default;
    BitStream& operator=(BitStream&&) = default;

    uint8_t* GetData()              { return m_Buffer.data(); }
    const uint8_t* GetData() const  { return m_Buffer.data(); }
    size_t GetBitLength() const     { return m_Buffer.size() * ByteInBits; }
    size_t GetByteLength() const    { return m_Buffer.size(); }
    size_t GetBitPosition() const   { return m_BitPosition; }
    void ResetBitPosition()         { m_BitPosition = 0; }
    void TruncateBytes()            { m_Buffer.resize((m_BitPosition + ByteInBits) / ByteInBits); }

    // Input stream
    template<typename T>    BitStream& Write(const T& value, size_t bitCount);
    template<typename T>    BitStream& operator<<(const T& value);
    template<>              BitStream& operator<<(const bool& value);

    // Output stream
    template<typename T>    BitStream& Read(T& value, size_t bitCount);
    template<typename T>    BitStream& operator>>(T& output);
    template<>              BitStream& operator>>(bool& output);

    BitStream  operator+(const BitStream& rhs) const;
    BitStream& operator+=(const BitStream& rhs);


private:

    static constexpr size_t ByteInBits = 8;

    std::vector<uint8_t> m_Buffer;
    size_t m_BitPosition;

};

template<typename T>    
BitStream& BitStream::Write(const T& value, size_t bitCount)
{
    // Lazy looping for now, might optimize in the future
    _ASSERT(bitCount > 0 && bitCount <= 64);
    if (bitCount > 0 && bitCount <= 64)
    {
        size_t bitPointer = 1ll << (bitCount - 1);
        
        while (bitPointer > 0)
        {
            (*this) << static_cast<bool>(value | bitPointer);
            bitPointer >>= 1;
        }
    }

    return *this;
}

template<typename T>
BitStream& BitStream::operator<<(const T& value)
{
    const size_t offset = m_BitPosition / ByteInBits;
    const size_t shiftRight = m_BitPosition % ByteInBits;
    const size_t shiftLeft = ByteInBits - shiftRight;

    // +1 to allow for bit overlap
    uint8_t data[sizeof(T) + 1] = { };

    std::memcpy(static_cast<void*>(&data[1]), static_cast<const void*>(&value), sizeof(T));

    T* vp = reinterpret_cast<T*>(&data[1]);

    for (size_t i = 0; i <= sizeof(T); i++)
    {
        if (i == sizeof(T))
        {
            data[i] = data[i] << shiftLeft;
        }
        else
        {
            data[i] = data[i] << shiftLeft | data[i + 1] >> shiftRight;
        }
    }

    // Expand if not enough space
    if (m_BitPosition + sizeof(T) * ByteInBits >= GetBitLength())
    {
        m_Buffer.resize(m_Buffer.size() + sizeof(T));
    }

    if (shiftRight == 0)
    {
        std::memcpy(static_cast<void*>(&m_Buffer[offset]), static_cast<void*>(&data[0]), sizeof(T));
    }
    else
    {
        m_Buffer[offset] |= data[0];

        std::memcpy(static_cast<void*>(&m_Buffer[offset + 1]), static_cast<void*>(&data[1]), sizeof(T));
    }
    m_BitPosition += sizeof(T) * ByteInBits;

    return *this;
}

template<>
BitStream& BitStream::operator<<(const bool& value)
{
    const size_t shift = ByteInBits - m_BitPosition % ByteInBits - 1;

    if (m_BitPosition % ByteInBits == 0)
    {
        m_Buffer.push_back(value ? 1 << shift : 0);
    }
    else
    {
        m_Buffer.back() |= (value ? 1 << shift : 0);
    }
    m_BitPosition++;

    return *this;
}

template<typename T>
BitStream& BitStream::Read(T& value, size_t bitCount)
{
    // Lazy looping for now, might optimize in the future
    _ASSERT(bitCount > 0 && bitCount <= 64);
    if (bitCount > 0 && bitCount <= 64)
    {
        size_t bitPointer = 1ll << (bitCount - 1);

        while (bitPointer > 0)
        {
            bool bitValue = false;
            (*this) >> bitValue;
            value |= bitValue ? bitPointer : 0;
            bitPointer >>= 1;
        }
    }

    return *this;
}


template<typename T>
BitStream& BitStream::operator>>(T& output)
{
    const size_t offset = m_BitPosition / ByteInBits;
    const size_t shiftLeft = m_BitPosition % ByteInBits;
    const size_t shiftRight = ByteInBits - shiftLeft;

    if (shiftRight == 0)
    {
        std::memcpy(static_cast<void*>(&output), static_cast<void*>(&m_Buffer[offset]), sizeof(T));
    }
    else
    {
        uint8_t data[sizeof(T) + 1] = { };

        std::memcpy(static_cast<void*>(data), static_cast<void*>(&m_Buffer[offset]), sizeof(data));

        for (size_t i = 0; i < sizeof(T); i++)
        {
            data[i] = data[i] << shiftLeft | data[i + 1] >> shiftRight;
        }

        std::memcpy(static_cast<void*>(&output), static_cast<void*>(data), sizeof(T));
    }

    m_BitPosition += sizeof(T) * ByteInBits;
    return *this;
}

template<> 
BitStream& BitStream::operator>>(bool& output)
{
    const size_t offset = m_BitPosition / ByteInBits;
    const size_t shift = ByteInBits - m_BitPosition % ByteInBits - 1;

    output = m_Buffer[offset] & (1 << shift);

    m_BitPosition++;

    return *this;
}