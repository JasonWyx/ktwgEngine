#pragma once

#include <vector>
#include <memory>
#include "singleton.h"

class Entity;
class Behaviour;

class KTWGBehaviour : public Singleton<KTWGBehaviour>
{
  template <typename T>
  using UniquePtr = std::unique_ptr<T>;

  template <typename T>
  using container_t = std::vector<T>;

public:
  KTWGBehaviour();
  ~KTWGBehaviour();

  void InitializeInternal() override;
  void ShutdownInternal() override;

  void Init();
  void Start();
  void Update();

  template <typename T>
  Behaviour* CreateBehaviour(Entity& entity);

private:
  container_t<UniquePtr<Behaviour>> m_Behaviours;
};

template<typename T>
inline Behaviour* KTWGBehaviour::CreateBehaviour(Entity& entity)
{
  m_Behaviours.emplace_back(std::make_unique<T>(entity));
  return m_Behaviours.back().get();
}
