#pragma once

#include <vector>
#include <memory>
#include "componenttype.h"
#include "component.h"

// forward declaration
class Entity;

template<typename T, unsigned SIZE>
struct ComponentManager
{
  template <typename T>
  using container_t = std::vector<T>;

  template <typename T>
  using SharedPtr = std::shared_ptr<T>;

  struct Usage
  {
    size_t nObjectsInUse;
    size_t nObjectInFreeList;
    size_t nPoolSize;
  };

  static container_t<T>&   GetList() { return ComponentManager::Instance().list_; }
  static ComponentManager& Instance() { static ComponentManager instance; return instance; }

  static SharedPtr<Component> Alloc(const SharedPtr<Entity>& entity, ComponentType type)
  {
    ComponentManager& instance = Instance();
    container_t<T>& list = Instance().GetList();
    if (instance.m_FreeList.empty())
    {
      list.emplace_back(entity, list.size());
      list.back().SetType(type);
      return std::make_shared<T>(list.back());
    }

    uint32_t index = instance.m_FreeList.back();
    instance.m_FreeList.pop_back();
    list[index].~T();
    new (&list[index]) T(entity, index);
    list[index].SetType(type);
    return std::make_shared<T>(list[index]);
  }

  static void Free(SharedPtr<Component> comp)
  {
    ComponentManager& instance = Instance();
    container_t<T>& list = Instance().GetList();
    uint32_t mid = comp->GetId();
    
    if (mid < list.size())
    {
      T& val = list[mid];
      val.SetActive(false);
      val.Destroy();
      instance.m_FreeList.emplace_back(mid);
    }
  }

  static Component& Get(SharedPtr<Component> comp)
  {
    ComponentManager& instance = Instance();
    return instance.m_List[comp->GetId()];
  }

  // Removes duplicates in freelist
  static void RefreshFreeList()
  {
    container_t<uint32_t>& list = Instance().m_FreeList;
    std::sort(list.begin(), list.end());
    list.erase(std::unique(list.begin(), list.end()), list.end());
  }

  static void Clear() { Instance().clear(); }
  
  void clear() 
  { 
    m_List.clear(); 
    m_FreeList.clear(); 
  }

  Usage QueryUsage() const
  {
    Usage u;
    u.nObjectInFreeList = m_FreeList.size();
    u.nObjectsInUse = m_List.size() - m_FreeList.size();
    u.nPoolSize = m_List.size();
    return u;
  }

private:
  ComponentManager()
    : m_List{}, m_FreeList{}
  {
    m_List.reserve(SIZE);
    m_FreeList.reserve(SIZE / 10);
  }

protected:
  container_t<T>        m_List;
  container_t<uint32_t> m_FreeList;
};
