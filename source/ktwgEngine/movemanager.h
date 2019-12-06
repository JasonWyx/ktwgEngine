#pragma once
#include "packet.h"

class MoveManager
{
public:

    MoveManager();
    ~MoveManager();

    void ReadStream(BitStream& stream);
    bool WritePacket(Packet& packet);
    void ResetTransmission();

private:

    

};