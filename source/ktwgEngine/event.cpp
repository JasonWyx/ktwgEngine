#include "event.h"
#include "behaviour.h"

std::unordered_map<PeerID, std::vector<Behaviour*>> BulletFireEvent::ms_Subscribers;
std::vector<Behaviour*> GameOverEvent::ms_Subscribers;

void BulletFireEvent::BulletFireEventHandler(Event * evt)
{
  BulletFireEvent* bulletFireEvent = (BulletFireEvent*)evt;
  PeerID peerID = bulletFireEvent->m_SourcePeerID;
  for (auto& behaviour : ms_Subscribers[peerID])
    behaviour->OnBulletFireEvent(bulletFireEvent);
}

void GameOverEvent::GameOverEventHandler(Event * evt)
{
  GameOverEvent* gameOverEvent = (GameOverEvent*)evt;
  for (auto& behaviour : ms_Subscribers)
    behaviour->OnGameOverEvent(gameOverEvent);
}
