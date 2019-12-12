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
#include "gamestatemanagerscript.h"

#include "gameplaymanager.h"

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
void Scene::CreateNewPlayer(PeerID peerID)
{
  static bool called = false;
  if(!called)
  {
    {
      // Player
      Entity* player = m_GameScene->AddChild();
      player->SetName("Player2");

      Transform& groundTF = player->GetTransform();
      groundTF.SetPosition(Vec3{ 20.f, 21.0f, 22.0f });
      groundTF.SetScale(Vec3{ 15.0f, 15.0f, 15.0f });

      CRigidBody& playerRB = player->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
      playerRB.SetBodyType(RBT_DYNAMIC);
      playerRB.SetFreezeRotationX(true);
      playerRB.SetFreezeRotationZ(true);

      CBoxCollider& boxABC = player->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

      CRenderable& renderable = player->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
      renderable.SetGraphicObject("Cube");
      renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
      renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);

      player->MarkEntityForGhost();
      GhostID id = player->GetGhostObject()->GetGhostID();
      StreamManager::GetInstance().GetGhostManager().ReplicateForAllPeer(id);

    }
    {
      // Player
      Entity* player = m_GameScene->AddChild();
      player->SetName("Player");

      Transform& groundTF = player->GetTransform();
      groundTF.SetPosition(Vec3{ 20.f, 21.0f, 22.0f });
      groundTF.SetScale(Vec3{ 10.0f, 10.0f, 10.0f });

      CRigidBody& playerRB = player->AddComponent(CT_RIGIDBODY)->Get<CRigidBody>();
      playerRB.SetBodyType(RBT_DYNAMIC);
      playerRB.SetFreezeRotationX(true);
      playerRB.SetFreezeRotationZ(true);

      CBoxCollider& boxABC = player->AddComponent(CT_BOXCOLLIDER)->Get<CBoxCollider>();

      CRenderable& renderable = player->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
      renderable.SetGraphicObject("Cube");
      renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
      renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

      
      CBehaviour& playerBeh = player->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
      playerBeh.Bind<PlayerController>();
      PlayerController* playerController = (PlayerController*)playerBeh.GetInternal();
      playerController->SetPeerID(peerID);
      playerController->CreateMoveControlObject();
      playerBeh.GetInternal()->Init();
      playerBeh.GetInternal()->Start();
      
      player->MarkEntityForGhost();
      GhostID id = player->GetGhostObject()->GetGhostID();
      StreamManager::GetInstance().GetGhostManager().ReplicateForAllPeer(id);

      called = true;

      GameplayManager::GetInstance().OnPlayerConnected(player);
    }
  }
}
#endif

void Scene::InitializeInternal()
{
  m_GameScene = Entity::CreateEntity("Scene");

  CreateStaticScene();
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

#if CLIENT
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

// #ifdef CLIENT
    CRenderable& renderable = ground->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
// #endif
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

// #ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.f, 0.25f, 0.25f, 1.0f);
// #endif
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

// #ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
// #endif
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

// #ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
// #endif
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

// #ifdef CLIENT
    CRenderable& renderable = wall->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable.SetGraphicObject("Cube");
    renderable.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 0.25f, 0.25f, 1.0f);
// #endif
  }

  {
    // EnemyManager
    Entity* enemyMng = m_GameScene->AddChild();
    enemyMng->SetName("enemyMng");

    CBehaviour& beh = enemyMng->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    beh.Bind<EnemyManager>();
  }

  {
    // Game State manager
    Entity* gameStateMng = m_GameScene->AddChild();
    gameStateMng->SetName("gameStateMng");

    CBehaviour& beh = gameStateMng->AddComponent(CT_BEHAVIOUR)->Get<CBehaviour>();
    beh.Bind<GameStateManager>();
  }

  {
    // Win State object
    Entity* win = m_GameScene->AddChild();
    win->SetName("Win");

    // Win - w0
    Entity* win_w0 = win->AddChild();
    Transform& win_w0Tf = win_w0->GetTransform();
    win_w0Tf.SetPosition(Vec3{ -6.23f, 50.0f, 0.05f });
    win_w0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });
    win_w0Tf.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 0.0f, 1.0f, 0.0f }, -15.0f));

    CRenderable& renderable_w0 = win_w0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_w0.SetGraphicObject("Cube");
    renderable_w0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_w0.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Win - w1
    Entity* win_w1 = win->AddChild();
    Transform& win_w1Tf = win_w1->GetTransform();
    win_w1Tf.SetPosition(Vec3{ -5.2f, 50.0f, -0.65f });
    win_w1Tf.SetScale(Vec3{ 1.0f, 1.0f, 3.0f });
    win_w1Tf.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 0.0f, 1.0f, 0.0f }, 15.0f));

    CRenderable& renderable_w1 = win_w1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_w1.SetGraphicObject("Cube");
    renderable_w1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_w1.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Win - w2
    Entity* win_w2 = win->AddChild();
    Transform& win_w2Tf = win_w2->GetTransform();
    win_w2Tf.SetPosition(Vec3{ -3.5f, 50.0f, -0.65f });
    win_w2Tf.SetScale(Vec3{ 1.0f, 1.0f, 3.0f });
    win_w2Tf.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 0.0f, 1.0f, 0.0f }, -15.0f));

    CRenderable& renderable_w2 = win_w2->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_w2.SetGraphicObject("Cube");
    renderable_w2.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_w2.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Win - w3
    Entity* win_w3 = win->AddChild();
    Transform& win_w3Tf = win_w3->GetTransform();
    win_w3Tf.SetPosition(Vec3{ -2.46f, 50.0f, 0.05f });
    win_w3Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });
    win_w3Tf.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 0.0f, 1.0f, 0.0f }, 15.0f));

    CRenderable& renderable_w3 = win_w3->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_w3.SetGraphicObject("Cube");
    renderable_w3.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_w3.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Win - i0
    Entity* win_i0 = win->AddChild();
    Transform& win_i0Tf = win_i0->GetTransform();
    win_i0Tf.SetPosition(Vec3{ 0.0f, 50.0f, 0.0f });
    win_i0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_i0 = win_i0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_i0.SetGraphicObject("Cube");
    renderable_i0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_i0.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Win - n0
    Entity* win_n0 = win->AddChild();
    Transform& win_n0Tf = win_n0->GetTransform();
    win_n0Tf.SetPosition(Vec3{ 2.2f, 50.0f, 0.0f });
    win_n0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_n0 = win_n0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_n0.SetGraphicObject("Cube");
    renderable_n0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_n0.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
    
    
    // Win - n1
    Entity* win_n1 = win->AddChild();
    Transform& win_n1Tf = win_n1->GetTransform();
    win_n1Tf.SetPosition(Vec3{ 3.86f, 50.0f, -0.04f });
    win_n1Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.5f });
    win_n1Tf.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 0.0f, 1.0f, 0.0f }, -35.0f));

    CRenderable& renderable_n1 = win_n1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_n1.SetGraphicObject("Cube");
    renderable_n1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_n1.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Win - n2
    Entity* win_n2 = win->AddChild();
    Transform& win_n2Tf = win_n2->GetTransform();
    win_n2Tf.SetPosition(Vec3{ 5.53f, 50.0f, -0.08f });
    win_n2Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_n2 = win_n2->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_n2.SetGraphicObject("Cube");
    renderable_n2.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_n2.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
  }

  {
    // Lose State object
    Entity* lose = m_GameScene->AddChild();
    lose->SetName("Lose");

    // Lose - l0
    Entity* lose_l0 = lose->AddChild();
    Transform& lose_l0Tf = lose_l0->GetTransform();
    lose_l0Tf.SetPosition(Vec3{ -9.64f, 50.0f, 0.0f });
    lose_l0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_l0 = lose_l0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_l0.SetGraphicObject("Cube");
    renderable_l0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_l0.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - l1
    Entity* lose_l1 = lose->AddChild();
    Transform& lose_l1Tf = lose_l1->GetTransform();
    lose_l1Tf.SetPosition(Vec3{ -7.64f, 50.0f, -2.0f });
    lose_l1Tf.SetScale(Vec3{ 3.0f, 1.0f, 1.0f });

    CRenderable& renderable_l1 = lose_l1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_l1.SetGraphicObject("Cube");
    renderable_l1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_l1.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - o0
    Entity* lose_o0 = lose->AddChild();
    Transform& lose_o0Tf = lose_o0->GetTransform();
    lose_o0Tf.SetPosition(Vec3{ -4.22f, 50.0f, 0.0f });
    lose_o0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_o0 = lose_o0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_o0.SetGraphicObject("Cube");
    renderable_o0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_o0.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - o1
    Entity* lose_o1 = lose->AddChild();
    Transform& lose_o1Tf = lose_o1->GetTransform();
    lose_o1Tf.SetPosition(Vec3{ -2.72f, 50.0f, 2.0f });
    lose_o1Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_o1 = lose_o1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_o1.SetGraphicObject("Cube");
    renderable_o1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_o1.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - o2
    Entity* lose_o2 = lose->AddChild();
    Transform& lose_o2Tf = lose_o2->GetTransform();
    lose_o2Tf.SetPosition(Vec3{ -1.22f, 50.0f, 0.0f });
    lose_o2Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_o2 = lose_o2->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_o2.SetGraphicObject("Cube");
    renderable_o2.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_o2.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - o3
    Entity* lose_o3 = lose->AddChild();
    Transform& lose_o3Tf = lose_o3->GetTransform();
    lose_o3Tf.SetPosition(Vec3{ -2.72f, 50.0f, -2.0f });
    lose_o3Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_o3 = lose_o3->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_o3.SetGraphicObject("Cube");
    renderable_o3.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_o3.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - s0
    Entity* lose_s0 = lose->AddChild();
    Transform& lose_s0Tf = lose_s0->GetTransform();
    lose_s0Tf.SetPosition(Vec3{ 2.85f, 50.0f, 2.0f });
    lose_s0Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_s0 = lose_s0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_s0.SetGraphicObject("Cube");
    renderable_s0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_s0.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - s1
    Entity* lose_s1 = lose->AddChild();
    Transform& lose_s1Tf = lose_s1->GetTransform();
    lose_s1Tf.SetPosition(Vec3{ 1.35f, 50.0f, 1.0f });
    lose_s1Tf.SetScale(Vec3{ 1.0f, 1.0f, 3.0f });

    CRenderable& renderable_s1 = lose_s1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_s1.SetGraphicObject("Cube");
    renderable_s1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_s1.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - s2
    Entity* lose_s2 = lose->AddChild();
    Transform& lose_s2Tf = lose_s2->GetTransform();
    lose_s2Tf.SetPosition(Vec3{ 2.35f, 50.0f, 0.0f });

    CRenderable& renderable_s2 = lose_s2->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_s2.SetGraphicObject("Cube");
    renderable_s2.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_s2.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - s3
    Entity* lose_s3 = lose->AddChild();
    Transform& lose_s3Tf = lose_s3->GetTransform();
    lose_s3Tf.SetPosition(Vec3{ 3.35f, 50.0f, -1.0f });
    lose_s3Tf.SetScale(Vec3{ 1.0f, 1.0f, 3.0f });

    CRenderable& renderable_s3 = lose_s3->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_s3.SetGraphicObject("Cube");
    renderable_s3.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_s3.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - s4
    Entity* lose_s4 = lose->AddChild();
    Transform& lose_s4Tf = lose_s4->GetTransform();
    lose_s4Tf.SetPosition(Vec3{ 1.85f, 50.0f, -2.0f });
    lose_s4Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_s4 = lose_s4->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_s4.SetGraphicObject("Cube");
    renderable_s4.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_s4.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - e0
    Entity* lose_e0 = lose->AddChild();
    Transform& lose_e0Tf = lose_e0->GetTransform();
    lose_e0Tf.SetPosition(Vec3{ 5.83f, 50.0f, 0.0f });
    lose_e0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_e0 = lose_e0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_e0.SetGraphicObject("Cube");
    renderable_e0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_e0.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - e1
    Entity* lose_e1 = lose->AddChild();
    Transform& lose_e1Tf = lose_e1->GetTransform();
    lose_e1Tf.SetPosition(Vec3{ 7.33f, 50.0f, 2.0f });
    lose_e1Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_e1 = lose_e1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_e1.SetGraphicObject("Cube");
    renderable_e1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_e1.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - e2
    Entity* lose_e2 = lose->AddChild();
    Transform& lose_e2Tf = lose_e2->GetTransform();
    lose_e2Tf.SetPosition(Vec3{ 7.33f, 50.0f, -0.2f });
    lose_e2Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_e2 = lose_e2->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_e2.SetGraphicObject("Cube");
    renderable_e2.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_e2.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);


    // Lose - e3
    Entity* lose_e3 = lose->AddChild();
    Transform& lose_e3Tf = lose_e3->GetTransform();
    lose_e3Tf.SetPosition(Vec3{ 7.33f, 50.0f, -2.0f });
    lose_e3Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_e3 = lose_e3->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_e3.SetGraphicObject("Cube");
    renderable_e3.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_e3.GetGraphicObjectInstance()->GetMaterial()->SetColor(1.0f, 0.5f, 0.5f, 1.0f);
  }

  {
    // Ready State object
    Entity* ready = m_GameScene->AddChild();
    ready->SetName("Ready");

    // Ready - r0
    Entity* ready_r0 = ready->AddChild();
    Transform& ready_r0Tf = ready_r0->GetTransform();
    ready_r0Tf.SetPosition(Vec3{ -1.44f, 50.0f, 0.0f });
    ready_r0Tf.SetScale(Vec3{ 1.0f, 1.0f, 5.0f });

    CRenderable& renderable_r0 = ready_r0->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_r0.SetGraphicObject("Cube");
    renderable_r0.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_r0.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // Ready - r1
    Entity* ready_r1 = ready->AddChild();
    Transform& ready_r1Tf = ready_r1->GetTransform();
    ready_r1Tf.SetPosition(Vec3{ 0.06f, 50.0f, 2.0f });
    ready_r1Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_r1 = ready_r1->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_r1.SetGraphicObject("Cube");
    renderable_r1.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_r1.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // ready - r2
    Entity* ready_r2 = ready->AddChild();
    Transform& ready_r2Tf = ready_r2->GetTransform();
    ready_r2Tf.SetPosition(Vec3{ 1.56f, 50.0f, 1.0f });
    ready_r2Tf.SetScale(Vec3{ 1.0f, 1.0f, 3.0f });

    CRenderable& renderable_r2 = ready_r2->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_r2.SetGraphicObject("Cube");
    renderable_r2.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_r2.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // ready - r3
    Entity* ready_r3 = ready->AddChild();
    Transform& ready_r3Tf = ready_r3->GetTransform();
    ready_r3Tf.SetPosition(Vec3{ 0.06f, 50.0f, 0.0f });
    ready_r3Tf.SetScale(Vec3{ 2.0f, 1.0f, 1.0f });

    CRenderable& renderable_r3 = ready_r3->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_r3.SetGraphicObject("Cube");
    renderable_r3.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_r3.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // ready - r4
    Entity* ready_r4 = ready->AddChild();
    Transform& ready_r4Tf = ready_r4->GetTransform();
    ready_r4Tf.SetPosition(Vec3{ 0.37f, 50.0f, -1.37f });
    ready_r4Tf.SetScale(Vec3{ 3.0f, 1.0f, 1.0f });
    ready_r4Tf.SetRotation(ConvertAxisAngleToQuaternion(Vec3{ 0.0f, 1.0f, 0.0f }, 15));

    CRenderable& renderable_r4 = ready_r4->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_r4.SetGraphicObject("Cube");
    renderable_r4.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_r4.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);


    // ready - r5
    Entity* ready_r5 = ready->AddChild();
    Transform& ready_r5Tf = ready_r5->GetTransform();
    ready_r5Tf.SetPosition(Vec3{ 1.45f, 50.0f, -1.87f });
    ready_r5Tf.SetScale(Vec3{ 1.0f, 1.0f, 1.2f });

    CRenderable& renderable_r5 = ready_r5->AddComponent(CT_RENDERABLE)->Get<CRenderable>();
    renderable_r5.SetGraphicObject("Cube");
    renderable_r5.GetGraphicObjectInstance()->CreateOverrideMaterial();
    renderable_r5.GetGraphicObjectInstance()->GetMaterial()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
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
