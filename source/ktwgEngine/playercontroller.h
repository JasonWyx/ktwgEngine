#pragma once

#include "behaviour.h"
#if SERVER
#include "movecontrolobject.h"
#endif

class GameStateManager;

class PlayerController : public Behaviour
{
  enum PlayerDirection {
    PD_FORWARD,
    PD_RIGHT,
    PD_BACKWARD,
    PD_LEFT,
    PD_MAX
  };

  enum DFlag {
    DIR_UP = 1,
    DIR_RIGHT = 1 << 1,
    DIR_DOWN = 1 << 2,
    DIR_LEFT = 1 << 3,
  };

public:
  PlayerController(Entity& entity);
  ~PlayerController();

  void Init() override;
  void Start() override;
  void Update() override;

  void OnCollisionEnter(Collision&);

  bool GetIsReady() const;
  bool GetIsAlive() const;
#if SERVER
  // Set PeerID should only be called once! and should precede the call to CreateMoveControlObject on the server
  void CreateMoveControlObject();
  void SetPeerID(PeerID peerID) { m_PeerID = peerID; }
  PeerID GetPeerID() const { return m_PeerID; }
  void OnBulletFireEvent(BulletFireEvent* bulletFireEvent) override;
  void OnPlayerReadyEvent(PlayerReadyEvent* playerReadyEvent) override;
#endif

private:
  void UpdateAction();
  void UpdateMovement();
  void UpdatePlayerStatus();

  void Fire();

  const float m_Speed = 20.0f;

  unsigned m_Attack = 1;

  Vec3 m_Directions[PD_MAX];
  uint8_t m_DirectionFlag;
  CRigidBody* m_Rb = nullptr;

  GameStateManager* m_GSManager;
  
  bool m_IsAlive;
  bool m_IsReady;

#if SERVER
  PeerID m_PeerID;
  MoveControlObject m_ControlObject;
#endif
};