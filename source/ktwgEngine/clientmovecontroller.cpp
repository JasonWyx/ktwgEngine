#include "clientmovecontroller.h"
#include "inputsystem.h"
#include "streammanager.h"

ClientMoveController::ClientMoveController(Entity & entity)
:Behaviour{typeid(ClientMoveController), entity}
, m_ShouldSend{false}
{
}

ClientMoveController::~ClientMoveController()
{
}

void ClientMoveController::Init()
{
}

void ClientMoveController::Start()
{
}

void ClientMoveController::Update()
{
#if CLIENT
  MoveState moveState;
  bool setThisFrame = false;

  if (Input().OnKeyDown(KTWG_UP))
  {
    moveState[MoveStateFlags::Up] = true;
    m_ShouldSend = true;
    setThisFrame = true;
  }
  if (Input().OnKeyDown(KTWG_DOWN))
  {
    moveState[MoveStateFlags::Down] = true;
    m_ShouldSend = true;
    setThisFrame = true;
  }
  if (Input().OnKeyDown(KTWG_LEFT))
  {
    moveState[MoveStateFlags::Left] = true;
    m_ShouldSend = true;
    setThisFrame = true;
  }
  if (Input().OnKeyDown(KTWG_RIGHT))
  {
    moveState[MoveStateFlags::Right] = true;
    m_ShouldSend = true;
    setThisFrame = true;
  }

  if (m_ShouldSend)
  {
    StreamManager::GetInstance().GetMoveManager().PushMoveState(moveState);
    if(!setThisFrame)
      m_ShouldSend = false;
  }
#endif
}
