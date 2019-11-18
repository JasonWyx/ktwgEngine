#pragma once

#include <vector>
#include <memory>
#include "componenttype.h"

// forward declaration
class Component;
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
    if (comp->m_Id < list.size())
    {
      T& val = list[comp->m_Id];
      val.SetActive(false);
      val.Destroy();
      instance.m_FreeList.emplace_back(comp->m_Id);
    }
  }

  static Component& Get(SharedPtr<Component> comp)
  {
    ComponentManager& instance = Instance();
    return instance.m_List[comp->m_Id];
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
    u.nObjectInFreeList = freelist_.size();
    u.nObjectsInUse = list_.size() - freelist_.size();
    u.nPoolSize = list_.size();
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
