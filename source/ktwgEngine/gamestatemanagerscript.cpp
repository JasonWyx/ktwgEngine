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
}

void GameStateManager::Init()
{
}

void GameStateManager::Start()
{
  m_WinObject = Scene::GetInstance().FindEntityByName("Win");
  m_LoseObject = Scene::GetInstance().FindEntityByName("Lose");

  m_WinObject->SetActive(false);
  m_LoseObject->SetActive(false);

  m_IsGameStarted = false;

  GameOverEvent::RegisterSubscriber(this);
}

void GameStateManager::Update()
{
  if (Input().OnKeyPress(KTWG_K))
    m_WinObject->SetActive(!m_WinObject->GetActive());

  if (Input().OnKeyPress(KTWG_L))
    m_LoseObject->SetActive(!m_LoseObject->GetActive());
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
}

bool GameStateManager::GetIsGameStarted() const
{
  return m_IsGameStarted;
}

void GameStateManager::SetIsGameStarted(bool start)
{
  m_IsGameStarted = start;
}

