#include "enemypoolscript.h"

#include "scene.h"
#include "entity.h"
#include "crigidbody.h"
#include "cboxcollider.h"
#include "crenderable.h"
#include "hypegraphicobject.h"

EnemyPool::EnemyPool()
  : ObjectPool{}
{
  Entity* obj = Scene().CreateEntity("enemy");
  
  Transform& tf = obj->GetTransform();
  tf.SetScale(Vec3{ 1.0f, 1.0f, 1.0f });
  
  CRigidBody& rb = obj->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
  rb.SetBodyType(RBT_DYNAMIC);
  rb.SetFreezeRotationX(true);
  rb.SetFreezeRotationZ(true);

  CBoxCollider& bc = obj->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

  CRenderable& renderable = obj->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
  renderable.SetGraphicObject("Cube");
  renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
  renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.75f, 0.25f, 0.25f, 1.0f);
  
  m_Object->SetActive(false);

  SetObject(obj);

  IncreasePool(10);
}

EnemyPool::~EnemyPool()
{
}
