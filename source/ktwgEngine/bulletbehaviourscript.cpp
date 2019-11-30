#include "bulletbehaviourscript.h"
#include "collision.h"

BulletBehaviour::BulletBehaviour(Entity& entity)
  : Behaviour{ typeid(BulletBehaviour), entity}
{
}

BulletBehaviour::~BulletBehaviour()
{
}

void BulletBehaviour::Start()
{
}

void BulletBehaviour::Update()
{
  GetComponent<CRigidBody>()->SetLinearVelocity(m_Speed * GetTransform().Forward());
}

void BulletBehaviour::OnTriggerEnter(Collision& collider)
{
  if (collider.entity->GetName() == "Player" || collider.entity->GetName() == "Bullet")
    return;

  m_Entity->SetActive(false);
}
