#include "gamestatemanagerscript.h"
#include "inputsystem.h"
#include "scene.h"
#include <iostream>

GameStateManager::GameStateManager(Entity& entity)
  : Behaviour{ typeid(GameStateManager), entity }
{
}

GameStateManager::~GameStateManager()
{
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
}

void GameStateManager::Update()
{
  if (Input().OnKeyPress(KTWG_K))
    m_WinObject->SetActive(!m_WinObject->GetActive());

  if (Input().OnKeyPress(KTWG_L))
    m_LoseObject->SetActive(!m_LoseObject->GetActive());
}
