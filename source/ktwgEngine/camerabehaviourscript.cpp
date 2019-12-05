#include "camerabehaviourscript.h"
#include "inputsystem.h"
#include "time.h"
#include "scene.h"
#include <iostream>

CameraBehaviour::CameraBehaviour(Entity& entity)
  : Behaviour{ typeid(CameraBehaviour), entity }
{
}

CameraBehaviour::~CameraBehaviour()
{
}

void CameraBehaviour::Start()
{
  m_Target = Scene::GetInstance().FindEntityByName("Player");
}

void CameraBehaviour::Update()
{
  Vec3 pos = GetTransform().GetPosition();
  
  if (m_Target)
  {
	  pos.x_ = m_Target->GetTransform().GetPosition().x_;
	  pos.z_ = m_Target->GetTransform().GetPosition().z_;
	  GetTransform().SetPosition(pos);
  }

  if (Input().OnKeyPress(KTWG_W))
  {
    pos.z_ -= 1.0f;
  }
  if (Input().OnKeyPress(KTWG_A))
  {
    pos.x_ -= 1.0f;
  }
  if (Input().OnKeyPress(KTWG_S))
  {
    pos.z_ += 1.0f;
  }
  if (Input().OnKeyPress(KTWG_D))
  {
    pos.x_ += 1.0f;
  }
  if (Input().OnKeyPress(KTWG_C))
  {
    pos.y_ -= 1.0f;
  }
  if (Input().OnKeyPress(KTWG_V))
  {
    pos.y_ += 1.0f;
  }

  GetTransform().SetPosition(pos);
}
