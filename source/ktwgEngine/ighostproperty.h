#pragma once
#include "bitstream.h"

class IGhostProperty
{
public:
    IGhostProperty() = default;
    virtual ~IGhostProperty() { }
    virtual void WriteStream(BitStream& stream) = 0;
    virtual void ReadStream(BitStream& stream) = 0;
};