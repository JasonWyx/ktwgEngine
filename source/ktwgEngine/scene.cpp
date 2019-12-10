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
#include "camerabehaviourscript.h"
#include "playercontroller.h"
#include "enemymanagerscript.h"
#include "clientmovecontroller.h"

// Network
#include "streammanager.h"

Scene::Scene()
  : m_GameScene{ nullptr }
{
}

Scene::~Scene()
{
}

Entity* Scene::CreateEntity(const std::string& name)
{
  Entity* entity = m_GameScene->AddChild();
  entity->SetName(name);
  return entity;
}

Entity* Scene::FindEntityByName(const std::string& name)
{
  return FindEntityByNameInternal(m_GameScene, name);
}

void Scene::CreateGhostEntity(BitStream & stream)
{
  // We need to create an entity from the bitstream
  // At this point we know an entity must be created
  Entity* entity = CreateEntity();
  entity->ReplicateGhostObjectFromBitstream(stream);
}

#if SERVER
void Scene::CreateNewPlayer()
{
  {
    // Player
    Entity* player = m_GameScene->AddChild();
    player->SetName("Player");

    Transform& groundTF = player->GetTransform();
    groundTF.SetPosition(Vec3{ 0.f, 2.0f, 0.0f });
    groundTF.SetScale(Vec3{ 1.0f, 1.0f, 1.0f });

    CRigidBody& playerRB = player->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    playerRB.SetBodyType(RBT_DYNAMIC);
    playerRB.SetFreezeRotationX(true);
    playerRB.SetFreezeRotationZ(true);

    CBoxCollider& boxABC = player->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

    CRenderable& renderable = player->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.5f, 0.25f, 0.25f, 1.0f);

    // CBehaviour& playerBeh = player->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    // playerBeh.Bind<PlayerController>();

    player->MarkEntityForGhost();
    StreamManager::GetInstance().GetGhostManager().ReplicateForAllPeer(player->GetGhostObject()->GetGhostID());
  }
}
#endif

void Scene::InitializeInternal()
{
  m_GameScene = Entity::CreateEntity("Scene");

  CreateStaticScene();
#if CLIENT
  {
    // Camera
    Entity* camera = m_GameScene->AddChild();
    camera->SetName("camera");

    Transform& cameraTF = camera->GetTransform();
    cameraTF.SetPosition(Vec3{ 0.f, 100.f, 0.f });
    cameraTF.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 1.0f, 0.0f, 0.0f }, 89.0f));

    CCamera& cameraCam = camera->AddComponent(CT_CAMERA)->Get<CCamera>();
    CBehaviour& camBeh = camera->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    camBeh.Bind<CameraBehaviour>();
  }

  {
    Entity* clientmovecontroller = m_GameScene->AddChild();
    CBehaviour& moveBeh = clientmovecontroller->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    moveBeh.Bind<ClientMoveController>();
  }
#endif
}

void Scene::ShutdownInternal()
{
}

void Scene::CreateStaticScene()
{
  {
    // Ground
    Entity* ground = m_GameScene->AddChild();
    ground->SetName("ground");

    Transform& groundTF = ground->GetTransform();
    groundTF.SetScale(Vec3{ 200.0f, 1.0f, 200.0f });

    CRigidBody& groundRB = ground->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    groundRB.SetBodyType(RBT_STATIC);

    CBoxCollider& groundBC = ground->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

#ifdef CLIENT
    CRenderable& renderable = ground->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
#endif
  }

  {
    // Wall
    Entity* wall = m_GameScene->AddChild();
    wall->SetName("wall");

    Transform& wallTf = wall->GetTransform();
    wallTf.SetPosition(Vec3{ 0.0f, 0.0f, 95.f });
    wallTf.SetScale(Vec3{ 200.0f, 200.0f, 10.0f });

    CRigidBody& wallRB = wall->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
    wallRB.SetBodyType(RBT_STATIC);

    CBoxCollider& wallBC = wall->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

#ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.f, 0.25f, 0.25f, 1.0f);
#endif
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

#ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
#endif
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

#ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
#endif
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

#ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
#endif
  }

  {
    // EnemyManager
    Entity* enemyMng = m_GameScene->AddChild();
    enemyMng->SetName("enemyMng");

    CBehaviour& beh = enemyMng->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    beh.Bind<EnemyManager>();
  }
}

Entity* Scene::FindEntityByNameInternal(Entity* ent, const std::string& name)
{
  if (ent)
  {
    if (ent->GetName() == name)
      return ent;

    for (auto& child : ent->GetChildren())
    {
      if (FindEntityByNameInternal(child, name))
        return child;
    }
  }

  return nullptr;
}
