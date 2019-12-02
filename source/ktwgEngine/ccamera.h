#pragma once

#include "component.h"

class Camera;

class CCamera : public Component
{

public:
  CCamera(Entity& owner, uint32_t id);
  ~CCamera();
  
  void Initialize() override;
  void Destroy() override;

private:
  Camera* m_Camera;
};
