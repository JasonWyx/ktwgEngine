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
  pos.x_ = m_Target->GetTransform().GetPosition().x_;
  pos.z_ = m_Target->GetTransform().GetPosition().z_;
  GetTransform().SetPosition(pos);
}
