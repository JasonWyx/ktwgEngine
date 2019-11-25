#include "scene.h"
#include "entity.h"

// Component
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"
#include "crenderable.h"
#include "ccamera.h"

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
  groundTF.SetScale(Vec3{ 50.0f, 50.0f, 50.0f });
  
  CRigidBody& groundRB = ground->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
  groundRB.SetBodyType(RBT_STATIC);
  
  CBoxCollider& groundBC = ground->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

  CBehaviour& groundBeh = ground->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
  groundBeh.Bind<TestBehaviour>();

  CRenderable& renderable = ground->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
  renderable.SetGraphicObject("Sphere");

  Entity* camera = m_GameScene->AddChild();
  camera->SetName("camera");

  Transform& cameraTF = camera->GetTransform();
  cameraTF.SetPosition(Vec3{0.f, 0.f, -70.f});

  CCamera& cameraCam = camera->AddComponent(CT_CAMERA)->Get<CCamera>();
}

void Scene::ShutdownInternal()
{
}
