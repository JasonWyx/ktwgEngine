#pragma once

#include "singleton.h"

class HypeRenderer : public Singleton<HypeRenderer>
{
public:
  void InitializeInternal() override;
  void ShutdownInternal() override;

  static void LoadSystemShaders();
  static void UnloadSystemShaders();
  static void CreateCommonResources();

private:
  static void LoadSimpleForward();
  static void UnloadSimpleForward();
};
