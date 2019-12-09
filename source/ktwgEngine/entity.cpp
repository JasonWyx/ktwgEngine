#include "entity.h"
#include "entitypool.h"

// Components' headers
#include "crigidbody.h"
#include "cboxcollider.h"
#include "cbehaviour.h"
#include "crenderable.h"
#include "ccamera.h"

// Network
#include "ghostobject.h"
#include "componentids.h"
#include "streammanager.h"

Entity::Entity(uint32_t id, const std::string& name)
  : m_Parent{ nullptr }, m_Children{}, m_Components{}, 
    m_Transform{}, m_State{ ACTIVE }, m_Id{ id }, m_Name{ name }, m_LayerId{ 0 }, m_IsGhost{false}, m_GhostObject{nullptr}
{
}

Entity::~Entity()
{
}

Entity* Entity::CreateEntity(const std::string& name)
{
  Entity& entity = EntityPool::Instance()->Create(name);
  return &entity;
}

Entity& Entity::GetEntity(uint32_t id)
{
  return EntityPool::Instance()->Get(id);
}

Entity* Entity::AddChild()
{
  Entity* ent = CreateEntity();
  m_Children.emplace_back(ent);
  return m_Children.back();
}

void Entity::AddChild(Entity* entity)
{
  m_Children.emplace_back(entity);
}

// @ ADD COMPONENT
Component* Entity::AddComponent(ComponentType type)
{
#define CASE_CREATE_COMP(comp_type, T) case comp_type: comp = &CreateComponent<T>(); 

  Component* comp = nullptr;
  switch (type)
  {
    //CASE_CREATE_COMP(NONE, TestComponent) break;
    CASE_CREATE_COMP(CT_RIGIDBODY, CRigidBody) break;
    CASE_CREATE_COMP(CT_BOXCOLLIDER, CBoxCollider) break;
    CASE_CREATE_COMP(CT_BEHAVIOUR, CBehaviour) break;
    CASE_CREATE_COMP(CT_RENDERABLE, CRenderable) break;
    CASE_CREATE_COMP(CT_CAMERA, CCamera) break;
    // ...
  default: break;
  }
  return comp;

#undef CASE_CREATE_COMP
}

void Entity::RemoveComponent(Component* comp)
{
  Component* tmp = comp;
  m_Components.erase(std::remove(m_Components.begin(), m_Components.end(), tmp), m_Components.end());
  Component::FreeComponent(comp);
}

void Entity::SetActive(bool active)
{
  m_State = active ? ACTIVE : INACTIVE;
  if (active)
  {
    SetAllComponentsActive(true);
    SetAllChildrenActive(true);
    return;
  }

  SetAllComponentsActive(false);
  SetAllChildrenActive(false);
}

void Entity::SetAllComponentsActive(bool active)
{
  for (auto& comp : m_Components)
    comp->SetActive(active);
}

void Entity::SetAllChildrenActive(bool active)
{
  for (auto& child : m_Children)
  {
    if (m_State != INACTIVE)
      child->SetActive(false);
  }
}

void Entity::Set(Entity* ent)
{
  m_State = ent->m_State;
  m_Name = ent->m_Name;
  m_LayerId = ent->m_LayerId;
  m_Transform = ent->m_Transform;

  for (auto& comp : ent->m_Components)
  {
    if (comp->GetType() != CT_BEHAVIOUR)
    {
      Component* newComp = AddComponent(comp->GetType());
      newComp->Set(comp);
    }
  }

  for (auto& child : ent->m_Children)
  {
    Entity* newChild = AddChild();
    newChild->Set(child);
  }

  if (ent->GetGhostObject() && !ent->GetIsGhost())
  {
    MarkEntityForGhost();
  }
}

void Entity::MarkEntityForGhost()
{
  m_GhostObject = new GhostObject{};
  m_GhostObject->SetGhostID(StreamManager::GetInstance().GetGhostManager().GetAvailableGhostID());
  m_GhostObject->SetPeerID(StreamManager::GetInstance().GetPeerID());
  m_IsGhost = false;

  // When we mark an entity for ghost we also need to set all its properties
  
}

void Entity::MarkEntityAsGhost(GhostID ghostId)
{
  // The difference is that this ghost is not owned by this peer
  m_GhostObject = new GhostObject{};
  m_GhostObject->SetGhostID(ghostId);
  m_IsGhost = true;
}

void Entity::ReplicateGhostObjectFromBitstream(BitStream & bitstream)
{
#if CLIENT
  NetAuthority netAuthority = NetAuthority::Server;
#else
  NetAuthority netAuthority = NetAuthority::Client;
#endif
  GhostID ghostId;
  bitstream >> ghostId;

  MarkEntityAsGhost(ghostId);

  // Basically reconstructs this entity from the bitstream
  uint8_t nameLen;
  bitstream >> nameLen;

  char buf[256]{};
  for (uint8_t i = 0; i < nameLen; ++i)
  {
    bitstream >> buf[i];
  }

  std::string name{buf};
  SetName(name);

  uint8_t numEntries;
  bitstream >> numEntries;

  // Sadly we can't do readstream here since the properties don't exist
  // We will have to manually unpack the packet here then
  for (uint8_t i = 0; i < numEntries; ++i)
  {
    ComponentID classID;
    bitstream >> classID;
    switch (classID)
    {
    case CI_Transform:
      m_GhostObject->RegisterPropertyCustom(new CustomGhostProperty(m_Transform, netAuthority));
      break;
    case CI_Component:
      {
        ComponentType cType;
        bitstream >> cType;
        Component* comp = AddComponent(cType);
        comp->RegisterAsGhostProperty(m_GhostObject, netAuthority);
        comp->GhostPropertyReadStream(bitstream);
      }
      break;
    }
  }
}

#if CLIENT
void Entity::ReplicateGhostObjectToBitstream(BitStream & bitstream)
{
  // ASSERT THAT THIS OBJECT HAS TO BE A GHOST
  bitstream << m_GhostObject->GetGhostID();
 
  uint8_t nameLen = (uint8_t)m_Name.length();
  bitstream << nameLen;

  for (uint8_t i = 0; i < nameLen; ++i)
  {
    bitstream << m_Name[i];
  }

  bitstream << (uint8_t)m_GhostObject->GetPropertyCount();

  // Write every property into the stream since it's a fresh replicate
  m_GhostObject->WriteStream(bitstream, m_GhostObject->GetFullStateMask());
}
#else
void Entity::ReplicateGhostObjectToBitstream(const PeerID targetPeerID, BitStream & bitstream)
{
  // ASSERT THAT THIS OBJECT HAS TO BE A GHOST
  bitstream << m_GhostObject->GetGhostID();

  uint8_t nameLen = (uint8_t)m_Name.length();
  bitstream << nameLen;

  for (uint8_t i = 0; i < nameLen; ++i)
  {
    bitstream << m_Name[i];
  }

  bitstream << (uint8_t)m_GhostObject->GetPropertyCount();

  // Write every property into the stream since it's a fresh replicate
  m_GhostObject->WriteStream(targetPeerID, bitstream, m_GhostObject->GetFullStateMask());
}
#endif