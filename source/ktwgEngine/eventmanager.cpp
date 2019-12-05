#include "eventmanager.h"

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::ReadPacket(Packet& packet)
{

}

bool EventManager::WritePacket(Packet& packet, TransmissionRecord& tr)
{
	return true;
}

void EventManager::NotifyTransmissionSuccess(TransmissionRecord& tr)
{
    // remove transmission list
}

void EventManager::NotifyTransmissionFailure(TransmissionRecord& tr)
{
    // append events again to broadcast list
}

void EventManager::BroadcastEvent(const Event& event, bool guaranteed)
{
    if (guaranteed)
    {
        m_GuaranteedEvents.push_back(event);
    }
    else
    {
        m_NonGuaranteedEvents.push_back(event);
    }
}
