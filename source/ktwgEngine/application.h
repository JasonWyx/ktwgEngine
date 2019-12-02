#pragma once

#include "singleton.h"

class Application : public Singleton<Application>
{
public:
  void InitializeInternal () override;
  void ShutdownInternal   () override;
  void Run                ();

private:
  void InitializeCoreSystems ();
  void InitializeResources   ();
  void LateInitialize        ();
};
