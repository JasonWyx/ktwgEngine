#pragma once

#include <memory>
#include <string>
#include "singleton.h"
#include "bitstream.h"
#include "netdefs.h"

class Entity;

class Scene : public Singleton <Scene>
{
public:
	Scene();
	~Scene();

  Entity* CreateEntity(const std::string& name = "GameObject");
  Entity* FindEntityByName(const std::string& name);

  void CreateGhostEntity(BitStream& stream);
#if SERVER
  void CreateNewPlayer(PeerID peerID);
#endif

private:
	virtual void InitializeInternal() override;
	virtual void ShutdownInternal() override;

  void CreateStaticScene();

  Entity* FindEntityByNameInternal(Entity* ent, const std::string& name);

  Entity* m_GameScene;
};