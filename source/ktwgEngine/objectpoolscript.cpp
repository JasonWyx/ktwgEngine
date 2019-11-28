#include "objectpoolscript.h"

#include "scene.h"

ObjectPool::ObjectPool(const TypeInfo& type, Entity& entity)
  : Behaviour{ type, entity }, m_Object{ nullptr }, m_Pool{}
{
}

ObjectPool::~ObjectPool()
{
}

void ObjectPool::Init()
{
}

void ObjectPool::Start()
{
}

void ObjectPool::Update()
{
}

void ObjectPool::IncreasePool(unsigned size)
{
  for (int i = 0; i < size; ++i)
  {
    Entity* clone = Scene().CreateEntity();
    clone->Set(m_Object);
    clone->SetActive(false);
    m_Pool.emplace_back(clone);
  }
}

Entity* ObjectPool::GetObject()
{
  for (auto& ent : m_Pool)
    if (!ent->GetActive())
      return ent;

  // All is being used, increment pool and return the first newly created object
  size_t currSize = m_Pool.size();
  IncreasePool(currSize);

  return m_Pool[currSize];
}
