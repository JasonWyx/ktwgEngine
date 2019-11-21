#pragma once

#include "entity.h"

class EntityPool
{
  template <typename T>
  using container_t = std::vector<T>;

public:
  struct Usage
  {
    size_t nObjectsInUse;
    size_t nObjectInFreeList;
    size_t nPoolSize;
  };

  static constexpr int MAX_ENTITY_COUNT = 1 << 17;
  static constexpr int FREELIST_RESERVE_COUNT = 10000;

  EntityPool();
  ~EntityPool();

  static EntityPool* Instance() { static EntityPool inst; return &inst; }

  Entity& Create(const std::string& name);

  inline Entity& Get(const size_t id) { return m_EntityTable[id]; }
  
  inline void Return(size_t id) { m_FreeList.emplace_back(id); }

  inline void Clear() { m_EntityTable.clear(); m_FreeList.clear(); }
  void RefreshFreeList();
  Usage QueryUsage() const;

private:
  container_t<Entity>   m_EntityTable;
  container_t<uint32_t> m_FreeList;
  uint32_t              m_Capacity;
};

inline Entity& GetEntity(const size_t id) { return EntityPool::Instance()->Get(id); }