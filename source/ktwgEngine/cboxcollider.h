#pragma once

#include <string>
#include "component.h"
#include "vector3.h"

class BoxCollider;
class RigidBody;

class CBoxCollider final : public Component
{
public:
  CBoxCollider(Entity& owner, uint32_t id);
  ~CBoxCollider();

  /* Override functions for abstract class */
  void Initialize() override;
  void Destroy()    override;
  void Set(Component* comp) override;

  bool Contains(const Vec3& point);

  /* getters */
  BoxCollider* GetInternal() const;
  Vec3 GetSize()       const;
  Vec3 GetCenter()     const;
  const Vec3& GetMin() const;
  const Vec3& GetMax() const;
  bool GetIsTrigger()  const;
  bool GetActive()     const;

  void SetRigidBody(RigidBody* rigidBody);
  void SetSize(const Vec3& size);
  void SetCenter(const Vec3& center);
  void SetIsTrigger(bool isTrigger);
  void SetActive(bool active) override;

private:
  BoxCollider* m_Internal;
};
