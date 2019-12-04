#pragma once

using EventID = unsigned char;
using EventSequenceID = unsigned char;
constexpr EventID INVALID_EVENT_ID = 0xFF;

struct Event
{
    Event(EventID eventID) : m_EventID(eventID), m_EventSequenceID(0) { };
    Event() : m_EventID(INVALID_EVENT_ID), m_EventSequenceID(0) { };
    ~Event() = default;

    EventID m_EventID;
    EventSequenceID m_EventSequenceID;
};