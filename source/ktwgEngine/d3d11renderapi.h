#pragma once

#include "singleton.h"
#include <d3d11.h>
#include <memory>

class D3D11Device;

class D3D11RenderAPI : public Singleton<D3D11RenderAPI>
{
public:
  void InitializeInternal () override;
  void ShutdownInternal   () override;

  D3D11Device* GetDevice() const { return m_Device; }
  
private:
  D3D11Device*                  m_Device;
  D3D_FEATURE_LEVEL             m_FeatureLevel;
};
