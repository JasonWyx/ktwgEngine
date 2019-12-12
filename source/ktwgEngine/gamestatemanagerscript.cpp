#include "gamestatemanagerscript.h"
#include "inputsystem.h"
#include "scene.h"
#include "event.h"

GameStateManager::GameStateManager(Entity& entity)
  : Behaviour{ typeid(GameStateManager), entity }
{
}

GameStateManager::~GameStateManager()
{
  GameOverEvent::RemoveSubscriber(this);
  GameStartEvent::RemoveSubscriber(this);
}

void GameStateManager::Init()
{
}

void GameStateManager::Start()
{
  m_WinObject = Scene::GetInstance().FindEntityByName("Win");
  m_LoseObject = Scene::GetInstance().FindEntityByName("Lose");
  m_ReadyObject = Scene::GetInstance().FindEntityByName("Ready");

  m_WinObject->SetActive(false);
  m_LoseObject->SetActive(false);
  m_ReadyObject->SetActive(false);

  m_IsGameStarted = false;

  GameOverEvent::RegisterSubscriber(this);
  GameStartEvent::RegisterSubscriber(this);
}

void GameStateManager::Update()
{
  if (Input().OnKeyPress(KTWG_K))
    m_WinObject->SetActive(!m_WinObject->GetActive());

  if (Input().OnKeyPress(KTWG_L))
    m_LoseObject->SetActive(!m_LoseObject->GetActive());

  if (Input().OnKeyPress(KTWG_J))
    m_ReadyObject->SetActive(!m_ReadyObject->GetActive());
}

void GameStateManager::OnGameOverEvent(GameOverEvent * evt)
{
  if (evt->m_Win)
  {
    m_WinObject->SetActive(true);
  }
  else
  {
    m_LoseObject->SetActive(true);
  }
}

void GameStateManager::OnGameStartEvent(GameStartEvent * evt)
{
  m_IsGameStarted = evt->m_Start;
  m_ReadyObject->SetActive(false);
}

bool GameStateManager::GetIsGameStarted() const
{
  return m_IsGameStarted;
}

void GameStateManager::SetIsGameStarted(bool start)
{
  m_IsGameStarted = start;
}

bool GameStateManager::GetReadyActive() const
{
  return m_ReadyObject->GetActive();
}

void GameStateManager::SetReadyActive(bool active)
{
  m_ReadyObject->SetActive(active);
}

