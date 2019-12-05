#pragma once
#include "packet.h"

class MoveManager
{
public:

    MoveManager();
    ~MoveManager();

    bool ReadPacket(Packet& packet);
    bool WritePacket(Packet& packet);
    void ResetTransmission();

private:

    

};