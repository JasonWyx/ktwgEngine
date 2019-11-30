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

void BulletBehaviour::OnTriggerEnter(Collision& other)
{
  if (other.entity->GetName() == "Player" || other.entity->GetName() == "Bullet")
    return;

  m_Entity->SetActive(false);
  
  if (other.entity->GetName() == "Enemy")
  {

  }
}
