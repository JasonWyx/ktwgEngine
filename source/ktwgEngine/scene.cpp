#include "scene.h"
#include "entity.h"

// Component
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"
#include "crenderable.h"
#include "ccamera.h"

#include "hypegraphicobject.h"

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

  {
    // Camera
    Entity* camera = m_GameScene->AddChild();
    camera->SetName("camera");

    Transform& cameraTF = camera->GetTransform();
    cameraTF.SetPosition(Vec3{ 0.f, 500.f, -200.f });
    cameraTF.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 1.0f, 0.0f, 0.0f }, 60.0f));

    CCamera& cameraCam = camera->AddComponent(CT_CAMERA)->Get<CCamera>();
  }

  {
    // Ground
    Entity* ground = m_GameScene->AddChild();
    ground->SetName("ground");

    Transform& groundTF = ground->GetTransform();
    groundTF.SetScale(Vec3{ 1000.0f, 100.0f, 1000.0f });

    CRigidBody& groundRB = ground->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    groundRB.SetBodyType(RBT_STATIC);

    CBoxCollider& groundBC = ground->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CBehaviour& groundBeh = ground->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    groundBeh.Bind<TestBehaviour>();

    CRenderable& renderable = ground->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");

  }

  {

    // BoxA
    Entity* boxA = m_GameScene->AddChild();
    boxA->SetName("boxA");

    Transform& groundTF = boxA->GetTransform();
    groundTF.SetPosition(Vec3{0.f, 200.0f, 0.0f});
    groundTF.SetScale(Vec3{ 100.0f, 100.0f, 100.0f });

    CRigidBody& boxARB = boxA->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    boxARB.SetBodyType(RBT_DYNAMIC);

    CBoxCollider& boxABC = boxA->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = boxA->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.5f, 0.25f, 0.25f, 1.0f);

  }

  {
    // BoxB
    Entity* boxB = m_GameScene->AddChild();
    boxB->SetName("boxB");

    Transform& boxBTfm = boxB->GetTransform();
    boxBTfm.SetPosition(Vec3{ -500.f, 100.0f, 0.0f });
    boxBTfm.SetScale(Vec3{ 100.0f, 100.0f, 100.0f });

    CRigidBody& boxBRB = boxB->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    boxBRB.SetBodyType(RBT_DYNAMIC);

    CBoxCollider& boxBCol = boxB->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = boxB->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.5f, 0.25f, 0.25f, 1.0f);

  }

}

void Scene::ShutdownInternal()
{
}
