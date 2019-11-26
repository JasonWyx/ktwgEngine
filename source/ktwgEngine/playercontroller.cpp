#include "playercontroller.h"
#include "inputsystem.h"
#include "time.h"

PlayerController::PlayerController(Entity& entity)
  : Behaviour{ typeid(PlayerController), entity }
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{

}

void PlayerController::Update()
{
  UpdateMovement();
}

void PlayerController::UpdateMovement()
{
  float dt = static_cast<float>(Time().GetDeltaTime());
  Vec3 movement;

  if (Input().OnKeyDown(KTWG_UP) && Input().OnKeyDown(KTWG_RIGHT))
  {
    movement += m_Speed * Vec3{ 1.0f, 0.0f, 1.0f };
    Transform().SetRotation(LookRotation(Vec3{ 1.0f, 0.0f, 1.0f }));
  }
  else if (Input().OnKeyDown(KTWG_UP))
  {
    movement += m_Speed * Vec3{ 0.0f, 0.0f, 1.0f };
    Transform().SetRotation(LookRotation(Vec3{ 0.0f, 0.0f, 1.0f }));
  }
  else if (Input().OnKeyDown(KTWG_DOWN))
  {
    movement += m_Speed * Vec3{ 0.0f, 0.0f, -1.0f };
    Transform().SetRotation(LookRotation(Vec3{ 0.0f, 0.0f, -1.0f }));
  }
  else if (Input().OnKeyDown(KTWG_LEFT))
  {
    movement += m_Speed * Vec3{ -1.0f, 0.0f, 0.0f };
    Transform().SetRotation(LookRotation(Vec3{ -1.0f, 0.0f, 0.0f }));
  }
  else if (Input().OnKeyDown(KTWG_RIGHT))
  {
    movement += m_Speed * Vec3{ 1.0f, 0.0f, 0.0f };
    Transform().SetRotation(LookRotation(Vec3{ 1.0f, 0.0f, 0.0f }));
  }

  GetComponent<CRigidBody>()->SetLinearVelocity(movement);
}
