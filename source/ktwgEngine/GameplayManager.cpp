#include "gameplaymanager.h"
#include "entity.h"
#include "playercontroller.h"
#include "streammanager.h"
#include "scene.h"
#include "gamestatemanagerscript.h"

Entity* GameplayManager::GetNearestPlayer(const Vec3 & position)
{
  float closestSqDist = FLT_MAX;
  int index = -1;
  for (int i = 0; i < (int)m_ConnectedPlayers.size(); ++i)
  {
    Vec3 pos = m_ConnectedPlayers[i]->GetTransform().GetPosition();
    float sqDist = SqDistance(pos, position);
    if (sqDist < closestSqDist)
    {
      closestSqDist = sqDist;
      index = i;
    }
  }
  return index == -1 ? nullptr : m_ConnectedPlayers[index];
}

Vec3 GameplayManager::GetNearestPlayerPosition(const Vec3 & position)
{
  float closestSqDist = FLT_MAX;
  Vec3 nearestPosition{};

  for (int i = 0; i < (int)m_ConnectedPlayers.size(); ++i)
  {
    Vec3 pos = m_ConnectedPlayers[i]->GetTransform().GetPosition();
    float sqDist = SqDistance(pos, position);
    if (sqDist < closestSqDist)
    {
      closestSqDist = sqDist;
      nearestPosition = pos;
    }
  }
  return nearestPosition;
}

void GameplayManager::OnPlayerConnected(Entity * player)
{
  if (std::find(m_ConnectedPlayers.begin(), m_ConnectedPlayers.end(), player) == m_ConnectedPlayers.end())
  {
    m_ConnectedPlayers.emplace_back(player);
  }
}

void GameplayManager::OnPlayerDisconnected(Entity * player)
{
  m_ConnectedPlayers.erase(std::remove(m_ConnectedPlayers.begin(), m_ConnectedPlayers.end(), player), m_ConnectedPlayers.end());
}

void GameplayManager::OnPlayerReady()
{
  bool allReady = true;
  for (auto& player : m_ConnectedPlayers)
  {
    PlayerController* playerBeh = player->GetComponent<PlayerController>();
    if (!playerBeh->GetIsReady())
    {
      allReady = false;
      break;
    }
  }

  if (allReady)
  {
    // Send start game
    GameStartEvent* evt = new GameStartEvent;
    evt->m_Start = true;
    StreamManager::GetInstance().GetEventManager().BroadcastEvent(evt, false);
    Scene::GetInstance().FindEntityByName("gameStateMng")->GetComponent<GameStateManager>()->SetIsGameStarted(true);
    ConnectionManager::GetInstance().ShutTheFuckUp();
  }
}

void GameplayManager::OnPlayerDeath()
{
  bool allDead = true;
  for (auto& player : m_ConnectedPlayers)
  {
    PlayerController* playerBeh = player->GetComponent<PlayerController>();
    if (playerBeh->GetIsAlive())
    {
      allDead = false;
      break;
    }
  }

  if (allDead)
  {
    // Send activate lose
    GameOverEvent* evt = new GameOverEvent;
    evt->m_Win = false;
    StreamManager::GetInstance().GetEventManager().BroadcastEvent(evt, false);
  }
}
