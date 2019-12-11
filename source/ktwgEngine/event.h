#pragma once
#include "netdefs.h"
#include "bitstream.h"

using EventSequenceID = unsigned char;

enum EventID
{
    EventID_Invalid = -1,
    EventID_BulletFire,
    EventID_GameStart,
    EventID_GameOver,
    EventID_Count
};

struct Event
{
    Event(EventID eventID = EventID_Invalid) : m_EventID(eventID) { };
    virtual ~Event() = default;
    virtual void ReadStream(BitStream& stream) { };
    virtual void WriteStream(BitStream& stream) { };

    EventID m_EventID;
    EventSequenceID m_EventSequenceID;
};

struct BulletFireEvent : public Event
{
    BulletFireEvent() : Event(EventID_BulletFire) { }

    virtual void ReadStream(BitStream& stream) override
    {
        stream >> m_SourcePeerID;
    }

    virtual void WriteStream(BitStream& stream) override
    {
        stream << m_SourcePeerID;
    }

    PeerID m_SourcePeerID;
};

struct GameStartEvent : public Event
{
    GameStartEvent() : Event(EventID_GameStart) { }
};

struct GameOverEvent : public Event
{
    GameOverEvent() : Event(EventID_GameOver) { }

    virtual void ReadStream(BitStream& stream) override
    {
        stream >> m_WinningPeer;
    }

    virtual void WriteStream(BitStream& stream) override
    {
        stream << m_WinningPeer;
    }
    
    PeerID m_WinningPeer;
};
