#include "entitypool.h"
#include <algorithm>

EntityPool::EntityPool()
  : m_EntityTable{}, m_FreeList{}, m_Capacity{}
{
  m_EntityTable.reserve(MAX_ENTITY_COUNT);
  m_FreeList.reserve(FREELIST_RESERVE_COUNT);
  m_Capacity = m_EntityTable.capacity();
}

EntityPool::~EntityPool()
{
}

Entity& EntityPool::Create(const std::string& name)
{
  if (m_FreeList.empty())
  {
    m_EntityTable.emplace_back(static_cast<uint32_t>(m_EntityTable.size()), name);
    Entity& entity = m_EntityTable.back();
    return entity;
  }

  uint32_t reuse = 0;
  Entity* entity = 0;

  do 
  {
    reuse = m_FreeList.back();
    m_FreeList.pop_back();

    entity = &m_EntityTable[reuse];

    // if entity is not dead, dont use it
  } while (!entity->IsDead());

  // destruct and reconstruct Entity
  entity->~Entity();
  new (entity) Entity(reuse, name);
  return *entity;
}

void EntityPool::RefreshFreeList()
{
  std::sort(m_FreeList.begin(), m_FreeList.end());
  m_FreeList.erase(std::unique(m_FreeList.begin(), m_FreeList.end()), m_FreeList.end());
}

EntityPool::Usage EntityPool::QueryUsage() const
{
  Usage u;
  u.nObjectInFreeList = m_FreeList.size();
  u.nObjectsInUse = m_EntityTable.size() - m_FreeList.size();
  u.nPoolSize = m_EntityTable.size();
  return u;
}
