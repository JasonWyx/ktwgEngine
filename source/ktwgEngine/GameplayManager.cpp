#include "gameplaymanager.h"
#include "entity.h"

Entity * GameplayManager::GetNearestPlayer(const Vec3 & position)
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
