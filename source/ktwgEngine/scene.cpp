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
    cameraTF.SetPosition(Vec3{ 0.f, 50.f, -2.f });
    cameraTF.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 1.0f, 0.0f, 0.0f }, 70.0f));

    CCamera& cameraCam = camera->AddComponent(CT_CAMERA)->Get<CCamera>();
    CBehaviour& camBeh = camera->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    camBeh.Bind<TestBehaviour>();
  }

  {
    // Ground
    Entity* ground = m_GameScene->AddChild();
    ground->SetName("ground");

    Transform& groundTF = ground->GetTransform();
    groundTF.SetScale(Vec3{ 200.0f, 1.0f, 200.0f });

    CRigidBody& groundRB = ground->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    groundRB.SetBodyType(RBT_STATIC);

    CBoxCollider& groundBC = ground->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = ground->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
  }

  {
    // Wall
    Entity* wall = m_GameScene->AddChild();
    wall->SetName("wall");

    Transform& wallTf = wall->GetTransform();
    wallTf.SetPosition(Vec3{0.0f, 0.0f, 95.f});
    wallTf.SetScale(Vec3{ 200.0f, 200.0f, 10.0f });

    CRigidBody& wallRB = wall->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    wallRB.SetBodyType(RBT_STATIC);

    CBoxCollider& wallBC = wall->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.f, 0.25f, 0.25f, 1.0f);
  }

  {
    // Wall
    Entity* wall = m_GameScene->AddChild();
    wall->SetName("wall");

    Transform& wallTf = wall->GetTransform();
    wallTf.SetPosition(Vec3{ 0.0f, 0.0f, -95.f });
    wallTf.SetScale(Vec3{ 200.0f, 200.0f, 10.0f });

    CRigidBody& wallRB = wall->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    wallRB.SetBodyType(RBT_STATIC);

    CBoxCollider& wallBC = wall->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
  }

  {
    // Wall
    Entity* wall = m_GameScene->AddChild();
    wall->SetName("wall");

    Transform& wallTf = wall->GetTransform();
    wallTf.SetPosition(Vec3{ 95.0f, 0.0f, 0.f });
    wallTf.SetScale(Vec3{ 10.0f, 200.0f, 200.0f });

    CRigidBody& wallRB = wall->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    wallRB.SetBodyType(RBT_STATIC);

    CBoxCollider& wallBC = wall->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
  }

  {
    // Wall
    Entity* wall = m_GameScene->AddChild();
    wall->SetName("wall");

    Transform& wallTf = wall->GetTransform();
    wallTf.SetPosition(Vec3{ -95.0f, 0.0f, 0.f });
    wallTf.SetScale(Vec3{ 10.0f, 200.0f, 200.0f });

    CRigidBody& wallRB = wall->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    wallRB.SetBodyType(RBT_STATIC);

    CBoxCollider& wallBC = wall->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
  }
  {
    // BoxA
    Entity* boxA = m_GameScene->AddChild();
    boxA->SetName("boxA");

    Transform& groundTF = boxA->GetTransform();
    groundTF.SetPosition(Vec3{0.f, 2.0f, 0.0f});
    groundTF.SetScale(Vec3{ 1.0f, 1.0f, 1.0f });

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
    boxBTfm.SetPosition(Vec3{ -5.f, 1.0f, 0.0f });
    boxBTfm.SetScale(Vec3{ 1.0f, 1.0f, 1.0f });

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
