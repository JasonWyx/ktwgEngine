#pragma once
#include "event.h"
#include "packet.h"
#include "transmissionrecord.h"
#include <list>
#include <map>
#include <memory>
#include <bitset>

#define REGISTER_NEW_EVENT(EventID, EventHandler) StreamManager::GetInstance().GetEventManager().AddEventHandler(EventID, EventHandler)

class EventManager
{
public:
    using EventHandler_t = void(*)(Event*);

    EventManager();
    ~EventManager();

    void ReadStream(const PeerID peerID, BitStream& stream);
    bool WritePacket(Packet& packet, TransmissionRecord& tr);
    void NotifyTransmissionSuccess(TransmissionRecord& tr);
    void NotifyTransmissionFailure(TransmissionRecord& tr);

    void BroadcastEvent(Event* event, bool guaranteed);
    void ProcessEvents();

    void AddEventHandler(EventID eventID, EventHandler_t handler);

    void RegisterEvents();
private:

    struct EventCache
    {
        EventSequenceID m_NextEventID = 0;
        std::list<Event*> m_NonGuaranteedEventsToBroadcast;
        std::list<Event*> m_GuaranteedEventsToBroadcast;
        std::list<Event*> m_EventsToRetransmit;

        EventSequenceID m_NextEventToProcess = 0;
        std::list<Event*> m_NonGuaranteedEventsToProcess;
        std::list<Event*> m_GuaranteedEventsToProcess;

        EventSequenceID m_EventsInFlight = 0;
        EventSequenceID m_EventsWindowStart = 0;
        std::bitset<64> m_AckedEvents;
    };

#ifdef CLIENT

    EventCache m_EventCache;

#else

    std::map<PeerID, EventCache> m_EventCache;

#endif
    std::map<EventID, std::vector<EventHandler_t>> m_EventListeners;

    unsigned m_TimesPacked[2];
    float m_ByteCount[2];


public:

    float m_AverageTimesPacked[2];
    float m_AverageByteCount[2];

    void PrintLog();
};