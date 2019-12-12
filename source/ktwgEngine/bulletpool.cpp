#include "bulletpool.h"
#include "scene.h"
#include "entity.h"
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"
#include "crenderable.h"
#include "hypegraphicobject.h"
#include "bulletbehaviourscript.h"
#include "streammanager.h"

BulletPool::BulletPool()
{
  m_Pool.reserve(1 << 13);

  m_Object = Scene::GetInstance().CreateEntity("Bullet");

  Transform& tf = m_Object->GetTransform();
  tf.SetScale(Vec3{ 0.5f, 0.5f, 0.5f });

  CRigidBody& rb = m_Object->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
  rb.SetBodyType(RBT_DYNAMIC);
  rb.SetMass(100.0f);
  rb.SetUseGravity(false);
  rb.SetFreezeRotationX(true);
  rb.SetFreezeRotationZ(true);

  CBoxCollider& bc = m_Object->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();
  bc.SetIsTrigger(true);

  CRenderable& renderable = m_Object->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
  renderable.SetGraphicObject("Cube");
  renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
  renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.75f, 0.75f, 0.25f, 1.0f);

  m_Object->SetActive(false);

  IncreasePool(2);
}

BulletPool::~BulletPool()
{
}

Entity* BulletPool::GetBullet()
{
  for (auto& obj : m_Pool)
  {
    if (!obj->GetActive())
      return obj;
  }

  size_t currSize = m_Pool.size();
  IncreasePool(static_cast<unsigned>(currSize));

  return m_Pool[currSize];
}

void BulletPool::IncreasePool(unsigned size)
{
  for (unsigned i = 0; i < size; ++i)
  {
    Entity* clone = Scene::GetInstance().CreateEntity();
    clone->Set(m_Object);

    CBehaviour& beh = clone->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    beh.Bind<BulletBehaviour>();

    clone->SetActive(false);
    m_Pool.emplace_back(clone);

#if SERVER
    clone->MarkEntityForGhost();
    GhostID id = clone->GetGhostObject()->GetGhostID();
    StreamManager::GetInstance().GetGhostManager().ReplicateForAllPeer(id);
#endif
  }
}
