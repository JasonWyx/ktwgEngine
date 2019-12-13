#include "clientmovecontroller.h"
#include "inputsystem.h"
#include "streammanager.h"
#include "scene.h"
#include "gamestatemanagerscript.h"
#include "time.h"

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
#if CLIENT
  m_GSManager = Scene::GetInstance().FindEntityByName("gameStateMng")->GetComponent<GameStateManager>();
#endif
}

void ClientMoveController::Update()
{
#if CLIENT
  if (!m_GSManager->GetIsGameStarted()) 
  {
    if (Input().OnKeyPress(KTWG_R))
    {
      PeerID peerID = StreamManager::GetInstance().GetPeerID();
      PlayerReadyEvent* playerReadyEvent = new PlayerReadyEvent;
      playerReadyEvent->m_SourcePeerID = peerID;
      playerReadyEvent->m_Ready = true;
      StreamManager::GetInstance().GetEventManager().BroadcastEvent(playerReadyEvent, true);
      m_GSManager->SetReadyActive(!m_GSManager->GetReadyActive());
    }
    return;
  }

  MoveState moveState;
  moveState.fill(false);

  if (Input().OnKeyDown(KTWG_UP))
  {
    moveState[MoveStateFlags::Up] = true;
  }
  if (Input().OnKeyDown(KTWG_DOWN))
  {
    moveState[MoveStateFlags::Down] = true;
  }
  if (Input().OnKeyDown(KTWG_LEFT))
  {
    moveState[MoveStateFlags::Left] = true;
  }
  if (Input().OnKeyDown(KTWG_RIGHT))
  {
    moveState[MoveStateFlags::Right] = true;
  }

  StreamManager::GetInstance().GetMoveManager().PushMoveState(moveState);
  
  if (Input().OnKeyPress(KTWG_SPACE))
  {
    if (m_CoolDown <= 0.0f)
    {
      PeerID peerID = StreamManager::GetInstance().GetPeerID();
      BulletFireEvent* bulletFireEvent = new BulletFireEvent;
      bulletFireEvent->m_SourcePeerID = peerID;
      StreamManager::GetInstance().GetEventManager().BroadcastEvent(bulletFireEvent, false);
      m_CoolDown = m_FireRate;
    }
  }

  m_CoolDown -= (float)Time().GetFixedDeltaTime();
#endif
}
