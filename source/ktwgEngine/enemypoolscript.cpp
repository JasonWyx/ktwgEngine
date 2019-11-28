#include "enemypoolscript.h"

#include "scene.h"

EnemyPool::EnemyPool(Entity& entity)
  : ObjectPool{ typeid(EnemyPool), entity }
{
  Entity* m_Object = Scene().CreateEntity("enemy");
  
  Transform& tf = m_Object->GetTransform();
  tf.SetScale(Vec3{ 1.0f, 1.0f, 1.0f });
  
  CRigidBody& rb = m_Object->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
  rb.SetBodyType(RBT_DYNAMIC);
  rb.SetFreezeRotationX(true);
  rb.SetFreezeRotationZ(true);

  CBoxCollider& bc = m_Object->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

  CRenderable& renderable = m_Object->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
  renderable.SetGraphicObject("Cube");
  renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
  renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.75f, 0.25f, 0.25f, 1.0f);
  
  m_Object->SetActive(false);

  IncreasePool(10);
}

EnemyPool::~EnemyPool()
{
}

void EnemyPool::Init()
{
}

void EnemyPool::Start()
{
}

void EnemyPool::Update()
{
}
