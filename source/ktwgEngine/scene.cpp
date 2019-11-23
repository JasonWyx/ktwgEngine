#include "scene.h"
#include "entity.h"

// Component
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"

// Behaviour
#include "TestBehaviour.h"

Scene::Scene()
  : m_GameScene{ nullptr }
{
}

Scene::~Scene()
{
}

void Scene::InitializeInternal()
{
  m_GameScene = Entity::CreateEntity("Scene");

  // Ground
  Entity* ground = m_GameScene->AddChild();
  ground->SetName("ground");

  Transform& groundTF = ground->GetTransform();
  groundTF.SetScale(Vec3{ 50.0f, 0.0f, 50.0f });
  
  CRigidBody& groundRB = ground->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
  groundRB.SetBodyType(RBT_STATIC);
  
  CBoxCollider& groundBC = ground->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

  CBehaviour& groundBeh = ground->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
  groundBeh.Bind<TestBehaviour>();
}

void Scene::ShutdownInternal()
{
}
