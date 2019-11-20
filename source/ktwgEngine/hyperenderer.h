#pragma once

#include "singleton.h"

class HypeRenderer : public Singleton<HypeRenderer>
{
public:
  void InitializeInternal() override;
  void ShutdownInternal() override;

  static void LoadSystemShaders();
  static void UnloadSystemShaders();
private:
  static void LoadSimpleForward();
  static void UnloadSimpleForward();
};
