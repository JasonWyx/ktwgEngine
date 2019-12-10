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

    void ReadStream(const PeerID peerID, BitStream& stream);
    bool WritePacket(Packet& packet, TransmissionRecord& tr);
    void NotifyTransmissionSuccess(TransmissionRecord& tr);
    void NotifyTransmissionFailure(TransmissionRecord& tr);

    void BroadcastEvent(Event* event, bool guaranteed);
    void ProcessEvents();


private:

    struct EventCache
    {
        EventSequenceID m_NextEventID = 0;
        EventSequenceID m_NextEventToBroadcast = 0;
        std::list<Event*> m_NonGuaranteedEventsToBroadcast;
        std::list<Event*> m_GuaranteedEventsToBroadcast;
        std::list<Event*> m_EventsToRetransmit;

        EventSequenceID m_NextEventToProcess = 0;
        std::list<Event*> m_NonGuaranteedEventsToProcess;
        std::list<Event*> m_GuaranteedEventsToProcess;

        unsigned m_CountEventsInFlight = 0;
        const unsigned m_MaxEventsInFlight = 128;
    };

#ifdef CLIENT

    EventCache m_EventCache;

#else

    std::map<PeerID, EventCache> m_EventCache;

#endif

};