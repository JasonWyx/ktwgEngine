#include "eventmanager.h"
#include "event.h"
#include "streammanager.h"
#include <algorithm>

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::ReadStream(const PeerID peerID, BitStream& stream)
{
#ifdef CLIENT
    EventCache& eventCache = m_EventCache;
#else
    EventCache& eventCache = m_EventCache[peerID];
#endif

    bool isGuaranteed = false;

    stream >> isGuaranteed;
    EventSequenceID sequence = 0;

    if (isGuaranteed)
    {
        stream >> sequence;
    }

    uint8_t numEvents = 0;
    stream.Read(numEvents, 3);
    numEvents++;
    for (uint8_t i = 0; i < numEvents; ++i)
    {
        // Reconstruct the event
        EventID eventID;
        stream >> eventID;

        Event* recvEvent = nullptr;
        switch (eventID)
        {
        case EventID_BulletFire:
            recvEvent = new BulletFireEvent;
            break;
        case EventID_GameStart:
            recvEvent = new GameStartEvent;
            break;
        case EventID_GameOver:
            recvEvent = new GameOverEvent;
            break;
        case EventID_PlayerReady:
            recvEvent = new PlayerReadyEvent;
            break;
        default:
            continue;
        }

        recvEvent->ReadStream(stream);

        if (isGuaranteed)
        {
            if (std::find_if(eventCache.m_GuaranteedEventsToProcess.begin(), eventCache.m_GuaranteedEventsToProcess.end(), [sequence](Event* evt) { return evt->m_EventSequenceID == sequence; }) != eventCache.m_GuaranteedEventsToProcess.end())
            {
                delete recvEvent;
            }
            else
            {
                recvEvent->m_EventSequenceID = sequence;
                eventCache.m_GuaranteedEventsToProcess.emplace_back(recvEvent);
            }
            ++sequence;
        }
        else
        {
            eventCache.m_NonGuaranteedEventsToProcess.emplace_back(recvEvent);
        }
    }

}

bool EventManager::WritePacket(Packet& packet, TransmissionRecord& tr)
{
    BitStream cumulatedEventStream;
    const size_t packetStreamSize = packet.GetStreamSize();
    unsigned char countEventsInPacket = 0;

#ifdef CLIENT
    EventCache& eventCache = m_EventCache;
#else
    EventCache& eventCache = m_EventCache[tr.m_TargetPeerID];
#endif

    // Pack non guaranteed events first is possible
    if (eventCache.m_NonGuaranteedEventsToBroadcast.size() > 0)
    {
        const size_t headerSize = 1 + 3; // Guarantee bit + packet count (3 bits)

        while (eventCache.m_NonGuaranteedEventsToBroadcast.size() > 0)
        {
            BitStream eventStream;
            if (Event* evt = eventCache.m_NonGuaranteedEventsToBroadcast.front())
            {
                eventStream << evt->m_EventID;
                evt->WriteStream(eventStream);

                if (eventStream.GetBitLength() + headerSize + packetStreamSize < MAX_PACKET_BIT_SIZE)
                {
                    eventCache.m_NonGuaranteedEventsToBroadcast.pop_front();
                    cumulatedEventStream += eventStream;
                    countEventsInPacket++;
                    
                    // Data logging
                    m_TimesPacked[0]++;

                    if (countEventsInPacket >= 8)
                    {
                        break;
                    }
                }
            }
        }

        // Pack what we have and break out return
        packet.m_EventStream << false;
        packet.m_EventStream.Write(countEventsInPacket - 1, 3);
        packet.m_EventStream += cumulatedEventStream;
        m_ByteCount[0] += packet.m_EventStream.GetByteLength();
    }
    else if (eventCache.m_EventsToRetransmit.size() > 0)
    {
        const size_t headerSize = 1 + 3 + 8; // Guarantee bit + packet count (3 bits) + sequence id
        const EventSequenceID firstSequenceID = eventCache.m_EventsToRetransmit.front()->m_EventSequenceID;

        while (eventCache.m_EventsToRetransmit.size() > 0)
        {
            BitStream eventStream;
            if (Event* evt = eventCache.m_EventsToRetransmit.front())
            {
                eventStream << evt->m_EventID;
                evt->WriteStream(eventStream);

                if (eventStream.GetBitLength() + headerSize + packetStreamSize < MAX_PACKET_BIT_SIZE)
                {
                    eventCache.m_EventsToRetransmit.pop_front();
                    cumulatedEventStream += eventStream;
                    countEventsInPacket++;

                    // Data logging
                    m_TimesPacked[1]++;

                    if (countEventsInPacket >= 8)
                    {
                        break;
                    }
                }
            }
        }

        packet.m_EventStream << true;
        packet.m_EventStream << firstSequenceID;
        packet.m_EventStream.Write(countEventsInPacket - 1, 3);
        packet.m_EventStream += cumulatedEventStream;
        m_ByteCount[1] += packet.m_EventStream.GetByteLength();
    }
    else if (eventCache.m_GuaranteedEventsToBroadcast.size() > 0)
    {
        const size_t headerSize = 1 + 3 + 8; // Guarantee bit + packet count (3 bits) + sequence id
        const EventSequenceID firstSequenceID = eventCache.m_GuaranteedEventsToBroadcast.front()->m_EventSequenceID;

        while (eventCache.m_GuaranteedEventsToBroadcast.size() > 0)
        {
            BitStream eventStream;
            if (Event* evt = eventCache.m_GuaranteedEventsToBroadcast.front())
            {
                eventStream << evt->m_EventID;
                evt->WriteStream(eventStream);

                if (eventStream.GetBitLength() + headerSize + packetStreamSize < MAX_PACKET_BIT_SIZE)
                {
                    eventCache.m_GuaranteedEventsToBroadcast.pop_front();
                    cumulatedEventStream += eventStream;
                    countEventsInPacket++;

                    // Data logging
                    m_TimesPacked[1]++;

                    if (countEventsInPacket >= 8)
                    {
                        break;
                    }
                }
            }
        }

        packet.m_EventStream << true;
        packet.m_EventStream << firstSequenceID;
        packet.m_EventStream.Write(countEventsInPacket - 1, 3);
        packet.m_EventStream += cumulatedEventStream;
        m_ByteCount[0] += packet.m_EventStream.GetByteLength();
    }
    else
    {
        // Nothing to pack
        return true;
    }

    if (eventCache.m_GuaranteedEventsToBroadcast.empty() &&
        eventCache.m_NonGuaranteedEventsToBroadcast.empty() &&
        eventCache.m_EventsToRetransmit.empty())
    {
        return true;
    }
    return false;
}

void EventManager::NotifyTransmissionSuccess(TransmissionRecord& tr)
{
    // remove transmission list
#ifdef CLIENT
    EventCache& eventCache = m_EventCache;
#else
    EventCache& eventCache = m_EventCache[tr.m_TargetPeerID];
#endif

    assert(tr.m_EventTransmissionRecords.size());

    EventSequenceID startingSequence = tr.m_EventTransmissionRecords.front()->m_EventSequenceID;
    
    size_t startingBitIndex = startingSequence - eventCache.m_EventsWindowStart;
    size_t endingBitIndex = startingBitIndex + tr.m_EventTransmissionRecords.size();

    for (size_t i = startingBitIndex; i < endingBitIndex; ++i)
    {
        eventCache.m_AckedEvents[i] = true;
    }

    size_t bitShift = 0;
    for (size_t i = 0; i < eventCache.m_AckedEvents.size(); i++)
    {
        if (eventCache.m_AckedEvents[i] == false)
        {
            bitShift = i;
        }
    }

    eventCache.m_AckedEvents >>= bitShift;
    eventCache.m_EventsWindowStart += bitShift;
}

void EventManager::NotifyTransmissionFailure(TransmissionRecord& tr)
{
    // append events again to broadcast list
#ifdef CLIENT
    m_EventCache.m_EventsToRetransmit = tr.m_EventTransmissionRecords;
#else
    m_EventCache[tr.m_TargetPeerID].m_EventsToRetransmit = tr.m_EventTransmissionRecords;
#endif
}

void EventManager::BroadcastEvent(Event* event, bool guaranteed)
{
    if (guaranteed)
    {
#ifdef CLIENT
        m_EventCache.m_GuaranteedEventsToBroadcast.push_back(event);
        m_EventCache.m_GuaranteedEventsToBroadcast.back()->m_EventSequenceID = m_EventCache.m_NextEventID++;
#else
        for (auto&[peerID, eventCache] : m_EventCache)
        {
            eventCache.m_GuaranteedEventsToBroadcast.push_back(event);
            eventCache.m_GuaranteedEventsToBroadcast.back()->m_EventSequenceID = eventCache.m_NextEventID++;
        }
#endif
    }
    else
    {
#ifdef CLIENT
        m_EventCache.m_NonGuaranteedEventsToBroadcast.push_back(event);
#else
        for (auto&[peerID, eventCache] : m_EventCache)
        {
            eventCache.m_NonGuaranteedEventsToBroadcast.push_back(event);
        }
#endif
    }
}

void EventManager::ProcessEvents()
{
#ifdef CLIENT
    while (m_EventCache.m_NonGuaranteedEventsToProcess.size() > 0)
    {
        Event* evt = m_EventCache.m_NonGuaranteedEventsToProcess.front();
        std::vector<EventHandler_t>& eventListeners = m_EventListeners[evt->m_EventID];

        for (auto& handler : eventListeners)
        {
            handler(evt);
        }
        delete evt;
        m_EventCache.m_NonGuaranteedEventsToProcess.pop_front();
    }

    while (m_EventCache.m_GuaranteedEventsToProcess.size() > 0)
    {
        Event* evt = m_EventCache.m_GuaranteedEventsToProcess.front();

        if (evt->m_EventSequenceID != m_EventCache.m_NextEventToProcess)
        {
            break;
        }
        std::vector<EventHandler_t>& eventListeners = m_EventListeners[evt->m_EventID];

        for (auto& handler : eventListeners)
        {
            handler(evt);
        }
        delete evt;
        m_EventCache.m_NextEventToProcess++;
        m_EventCache.m_GuaranteedEventsToProcess.pop_front();
    }

#else

    for (auto&[peerID, eventCache] : m_EventCache)
    {
        while (eventCache.m_NonGuaranteedEventsToProcess.size() > 0)
        {
            Event* evt = eventCache.m_NonGuaranteedEventsToProcess.front();
            std::vector<EventHandler_t>& eventListeners = m_EventListeners[evt->m_EventID];

            for (auto& handler : eventListeners)
            {
                handler(evt);
            }
            delete evt;
            eventCache.m_NonGuaranteedEventsToProcess.pop_front();
        }

        eventCache.m_GuaranteedEventsToProcess.sort([](Event* lhs, Event* rhs)
        {
            return lhs->m_EventSequenceID < rhs->m_EventSequenceID;
        });

        while (eventCache.m_GuaranteedEventsToProcess.size() > 0)
        {
            Event* evt = eventCache.m_GuaranteedEventsToProcess.front();

            if (evt->m_EventSequenceID != eventCache.m_NextEventToProcess)
            {
                break;
            }
            std::vector<EventHandler_t>& eventListeners = m_EventListeners[evt->m_EventID];

            for (auto& handler : eventListeners)
            {
                handler(evt);
            }
            delete evt;
            eventCache.m_NextEventToProcess++;
            eventCache.m_GuaranteedEventsToProcess.pop_front();
        }
    }
#endif
}

void EventManager::AddEventHandler(EventID eventID, EventHandler_t handler)
{
    m_EventListeners[eventID].emplace_back(handler);
}

void EventManager::RegisterEvents()
{
    REGISTER_NEW_EVENT(EventID_BulletFire, &BulletFireEvent::BulletFireEventHandler);
    REGISTER_NEW_EVENT(EventID_GameOver, &GameOverEvent::GameOverEventHandler);
    REGISTER_NEW_EVENT(EventID_GameStart, &GameStartEvent::GameStartEventEventHandler);
    REGISTER_NEW_EVENT(EventID_PlayerReady, &PlayerReadyEvent::PlayerReadyEventHandler);
}

void EventManager::PrintLog()
{
    std::cout << "    Times Packed (non-guaranteed): " << m_TimesPacked[0] << std::endl;
    std::cout << "    Times Packed (guaranteed)    : " << m_TimesPacked[1] << std::endl;
    std::cout << "    Bytes Used (non-guaranteed)  : " << m_ByteCount[0] << std::endl;
    std::cout << "    Bytes Used (guaranteed)      : " << m_ByteCount[1] << std::endl;

    m_AverageTimesPacked[0] += m_TimesPacked[0];
    m_AverageTimesPacked[1] += m_TimesPacked[1];
    m_AverageByteCount[0] += m_ByteCount[0];
    m_AverageByteCount[1] += m_ByteCount[1];

    m_AverageTimesPacked[0] /= 2;
    m_AverageTimesPacked[1] /= 2;
    m_AverageByteCount[0] /= 2;
    m_AverageByteCount[1] /= 2;

    m_TimesPacked[0] = 0;
    m_TimesPacked[1] = 0;
    m_ByteCount[0] = 0;
    m_ByteCount[1] = 0;
}
