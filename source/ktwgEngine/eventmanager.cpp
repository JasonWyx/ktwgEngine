#include "eventmanager.h"

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::ReadStream(const PeerID peerID, BitStream& stream)
{
    bool isGuaranteed = false;

    stream >> isGuaranteed;

    if (isGuaranteed)
    {
        unsigned char sequence = 0;
        stream >> sequence;

    }


#ifdef CLIENT

#else

#endif
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
                    eventCache.m_GuaranteedEventsToBroadcast.pop_front();
                    cumulatedEventStream += eventStream;
                    countEventsInPacket++; 

                    if (countEventsInPacket > 8)
                    {
                        // Pack what we have and break out return
                        packet.m_EventStream << false;
                        packet.m_EventStream.Write(countEventsInPacket, 3);
                        packet.m_EventStream += cumulatedEventStream;
                        break;
                    }
                }
            }
        }
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

                    if (countEventsInPacket > 8)
                    {
                        packet.m_EventStream << false;
                        packet.m_EventStream << firstSequenceID;
                        packet.m_EventStream.Write(countEventsInPacket, 3);
                        packet.m_EventStream += cumulatedEventStream;
                        break;
                    }
                }
            }
        }
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

                    if (countEventsInPacket > 8)
                    {
                        packet.m_EventStream << false;
                        packet.m_EventStream << firstSequenceID;
                        packet.m_EventStream.Write(countEventsInPacket, 3);
                        packet.m_EventStream += cumulatedEventStream;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // Nothing to pack
        return true;
    }

    if (eventCache.m_GuaranteedEventsToBroadcast.empty() && eventCache.m_NonGuaranteedEventsToBroadcast.empty())
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

    
}

void EventManager::NotifyTransmissionFailure(TransmissionRecord& tr)
{
    // append events again to broadcast list
#ifdef CLIENT
    m_EventCache.m_EventsToRetransmit = tr.m_Events;
#else
    m_EventCache[tr.m_TargetPeerID].m_EventsToRetransmit = tr.m_Events;
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
        switch (evt->m_EventID)
        {
        case EventID_BulletFire:
            //BulletFireEvent * bulletEvent = dynamic_cast<BulletFireEvent*>(evt);
            break;
        case EventID_GameStart:
            //GameStartEvent * gameStartEvent = dynamic_cast<GameStartEvent*>(evt);
            break;
        case EventID_GameOver:
            //GameOverEvent * gameOverEvent = dynamic_cast<GameOverEvent*>(evt);
            break;
        default:
            break;

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

        switch (evt->m_EventID)
        {
        case EventID_BulletFire:
            //BulletFireEvent * bulletEvent = dynamic_cast<BulletFireEvent*>(evt);
            break;
        case EventID_GameStart:
            //GameStartEvent * gameStartEvent = dynamic_cast<GameStartEvent*>(evt);
            break;
        case EventID_GameOver:
            //GameOverEvent * gameOverEvent = dynamic_cast<GameOverEvent*>(evt);
            break;
        default:
            break;
        }
        delete evt;
        m_EventCache.m_NextEventToProcess++;
        m_EventCache.m_NonGuaranteedEventsToProcess.pop_front();
    }

#else

    for (auto&[peerID, eventCache] : m_EventCache)
    {
        while (eventCache.m_NonGuaranteedEventsToProcess.size() > 0)
        {
            Event* evt = eventCache.m_NonGuaranteedEventsToProcess.front();
            switch (evt->m_EventID)
            {
            case EventID_BulletFire:
                //BulletFireEvent * bulletEvent = dynamic_cast<BulletFireEvent*>(evt);
                break;
            case EventID_GameStart:
                //GameStartEvent * gameStartEvent = dynamic_cast<GameStartEvent*>(evt);
                break;
            case EventID_GameOver:
                //GameOverEvent * gameOverEvent = dynamic_cast<GameOverEvent*>(evt);
                break;
            default:
                break;

            }
            delete evt;
            eventCache.m_NonGuaranteedEventsToProcess.pop_front();
        }

        while (eventCache.m_GuaranteedEventsToProcess.size() > 0)
        {
            Event* evt = eventCache.m_GuaranteedEventsToProcess.front();

            if (evt->m_EventSequenceID != eventCache.m_NextEventToProcess)
            {
                break;
            }

            switch (evt->m_EventID)
            {
            case EventID_BulletFire:
                //BulletFireEvent * bulletEvent = dynamic_cast<BulletFireEvent*>(evt);
                break;
            case EventID_GameStart:
                //GameStartEvent * gameStartEvent = dynamic_cast<GameStartEvent*>(evt);
                break;
            case EventID_GameOver:
                //GameOverEvent * gameOverEvent = dynamic_cast<GameOverEvent*>(evt);
                break;
            default:
                break;
            }
            delete evt;
            eventCache.m_NextEventToProcess++;
            eventCache.m_NonGuaranteedEventsToProcess.pop_front();
        }
    }
#endif
}
