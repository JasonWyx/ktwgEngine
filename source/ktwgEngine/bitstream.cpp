#include "bitstream.h"

BitStream::BitStream() : BitStream(0)
{
}

BitStream::BitStream(size_t bytes)
    : m_Buffer(bytes)
    , m_BitPosition(0)
{
    
}
