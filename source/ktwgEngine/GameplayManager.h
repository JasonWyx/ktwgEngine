#pragma once

#include "singleton.h"
#include "vector3.h"
#include <vector>

class Entity;

class GameplayManager : public Singleton<GameplayManager>
{
public:
  virtual void InitializeInternal() override {};
  virtual void ShutdownInternal() override {};

  Entity* GetNearestPlayer(const Vec3& position);
  Vec3 GetNearestPlayerPosition(const Vec3& position);

  const std::vector<Entity*>& GetConnectedPlayers() const { return m_ConnectedPlayers; }

  void OnPlayerConnected(Entity* player);
  void OnPlayerDisconnected(Entity* player);
  
  void OnPlayerDeath();

private:
  std::vector<Entity*> m_ConnectedPlayers;
};