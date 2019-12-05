#include "enemypool.h"

#include "scene.h"
#include "entity.h"
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"
#include "hypegraphicobject.h"

#include "crenderable.h"
#include "enemybehaviourscript.h"

EnemyPool::EnemyPool()
{
  m_Object = Scene::GetInstance().CreateEntity("Enemy");
  
  Transform& tf = m_Object->GetTransform();
  tf.SetScale(Vec3{ 1.0f, 1.0f, 1.0f });
  
  CRigidBody& rb = m_Object->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
  rb.SetBodyType(RBT_DYNAMIC);
  rb.SetMass(100.0f);
  rb.SetFreezeRotationX(true);
  rb.SetFreezeRotationZ(true);

  CBoxCollider& bc = m_Object->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

#ifdef CLIENT
  CRenderable& renderable = m_Object->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
  renderable.SetGraphicObject("Cube");
  renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
  renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.75f, 0.25f, 0.25f, 1.0f);
#endif

  m_Object->SetActive(false);

  IncreasePool(10);
}

EnemyPool::~EnemyPool()
{
}

void EnemyPool::IncreasePool(unsigned size)
{
	for (unsigned i = 0; i < size; ++i)
	{
		Entity* clone = Scene::GetInstance().CreateEntity();
		clone->Set(m_Object);

    CBehaviour& beh = clone->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    beh.Bind<EnemyBehaviour>();

		clone->SetActive(false);
		m_Pool.emplace_back(clone);
	}
}

Entity* EnemyPool::GetEnemy()
{
  for (auto& ent : m_Pool)
  {
		if (!ent->GetActive())
			return ent;
  }

	// All is being used, increment pool and return the first newly created object
	unsigned currSize = static_cast<unsigned>(m_Pool.size());
	IncreasePool(currSize);

	return m_Pool[currSize];
}

