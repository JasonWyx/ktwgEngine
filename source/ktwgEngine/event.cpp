#include "event.h"
#include "behaviour.h"

std::unordered_map<PeerID, std::vector<Behaviour*>> BulletFireEvent::ms_Subscribers;
void BulletFireEvent::BulletFireEventHandler(Event * evt)
{
  BulletFireEvent* bulletFireEvent = (BulletFireEvent*)evt;
  PeerID peerID = bulletFireEvent->m_SourcePeerID;
  for (auto& behaviour : ms_Subscribers[peerID])
    behaviour->OnBulletFireEvent(bulletFireEvent);
}
