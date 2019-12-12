#pragma once
#include "netdefs.h"
#include "bitstream.h"
#include <unordered_map>
#include <algorithm>

using EventSequenceID = unsigned char;

class Behaviour;

enum EventID
{
    EventID_Invalid = -1,
    EventID_BulletFire,
    EventID_GameStart,
    EventID_GameOver,
    EventID_PlayerReady,
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

    static void BulletFireEventHandler(Event* evt);
    
    static void RegisterSubscriber(PeerID peerID, Behaviour* behaviour)
    {
      ms_Subscribers[peerID].emplace_back(behaviour);
    }

    static void RemoveSubscriber(PeerID peerID, Behaviour* behaviour)
    {
      std::vector<Behaviour*>& subscribers = ms_Subscribers[peerID];
      subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), behaviour), subscribers.end());
    }

    static std::unordered_map<PeerID, std::vector<Behaviour*>> ms_Subscribers;
    PeerID m_SourcePeerID;
};

struct GameStartEvent : public Event
{
    GameStartEvent() : Event(EventID_GameStart) { }

    virtual void ReadStream(BitStream& stream) override
    {
      stream >> m_Start;
    }

    virtual void WriteStream(BitStream& stream) override
    {
      stream << m_Start;
    }

    static void GameStartEventEventHandler(Event* evt);

    static void RegisterSubscriber(Behaviour* behaviour)
    {
      ms_Subscribers.emplace_back(behaviour);
    }

    static void RemoveSubscriber(Behaviour* behaviour)
    {
      ms_Subscribers.erase(std::remove(ms_Subscribers.begin(), ms_Subscribers.end(), behaviour), ms_Subscribers.end());
    }

    static std::vector<Behaviour*> ms_Subscribers;
    bool m_Start;
};

struct GameOverEvent : public Event
{
    GameOverEvent() : Event(EventID_GameOver) { }

    virtual void ReadStream(BitStream& stream) override
    {
      stream >> m_Win;
    }

    virtual void WriteStream(BitStream& stream) override
    {
      stream << m_Win;
    }

    static void GameOverEventHandler(Event* evt);

    static void RegisterSubscriber(Behaviour* behaviour)
    {
      ms_Subscribers.emplace_back(behaviour);
    }

    static void RemoveSubscriber(Behaviour* behaviour)
    {
      ms_Subscribers.erase(std::remove(ms_Subscribers.begin(), ms_Subscribers.end(), behaviour), ms_Subscribers.end());
    }

    static std::vector<Behaviour*> ms_Subscribers;
    bool m_Win;
};

struct PlayerReadyEvent : public Event
{
  PlayerReadyEvent() : Event(EventID_PlayerReady) { }

  virtual void ReadStream(BitStream& stream) override
  {
    stream >> m_SourcePeerID;
    stream >> m_Ready;
  }

  virtual void WriteStream(BitStream& stream) override
  {
    stream << m_SourcePeerID;
    stream << m_Ready;
  }

  static void PlayerReadyEventHandler(Event* evt);

  static void RegisterSubscriber(PeerID peerID, Behaviour* behaviour)
  {
    ms_Subscribers[peerID].emplace_back(behaviour);
  }

  static void RemoveSubscriber(PeerID peerID, Behaviour* behaviour)
  {
    std::vector<Behaviour*>& subscribers = ms_Subscribers[peerID];
    subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), behaviour), subscribers.end());
  }

  static std::unordered_map<PeerID, std::vector<Behaviour*>> ms_Subscribers;
  PeerID m_SourcePeerID;
  bool m_Ready;
};
