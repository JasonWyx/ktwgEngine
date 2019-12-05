#pragma once
#include "event.h"
#include "packet.h"
#include "transmissionrecord.h"
#include <list>
#include <map>
#include <memory>

class EventManager
{
public:

    EventManager();
    ~EventManager();

    void ReadPacket(Packet& packet);
    bool WritePacket(Packet& packet, TransmissionRecord& tr);
    void NotifyTransmissionSuccess(TransmissionRecord& tr);
    void NotifyTransmissionFailure(TransmissionRecord& tr);

    void BroadcastEvent(const Event& event, bool guaranteed);

private:

    std::list<Event> m_NonGuaranteedEvents;
    std::list<Event> m_GuaranteedEvents;
};