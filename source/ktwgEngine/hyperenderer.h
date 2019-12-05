#pragma once

#include "singleton.h"
#include <vector>

class HypeRenderNode;
class D3D11RenderWindow;

class HypeRenderer : public Singleton<HypeRenderer>
{
public:
  void InitializeInternal() override;
  void ShutdownInternal() override;

  void SetupRenderNodes();

  static void LoadSystemShaders();
  static void UnloadSystemShaders();
  static void CreateCommonResources();

  void Update();

private:
  static void LoadSimpleForward();
  static void UnloadSimpleForward();

  std::vector<HypeRenderNode*> m_RenderNodes;
  D3D11RenderWindow* m_RenderWindow;
};
