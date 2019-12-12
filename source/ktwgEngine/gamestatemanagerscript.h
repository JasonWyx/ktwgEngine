#pragma once

#include "behaviour.h"

class Entity;

class GameStateManager : public Behaviour
{
public:
  GameStateManager(Entity& entity);
  ~GameStateManager();

  void Init() override;
  void Start() override;
  void Update() override;

  void OnGameOverEvent(GameOverEvent* evt) override;
  void OnGameStartEvent(GameStartEvent* evt) override;

  bool GetIsGameStarted() const;
  void SetIsGameStarted(bool start);

private:
  Entity* m_WinObject;
  Entity* m_LoseObject;

  bool m_IsGameStarted;
};