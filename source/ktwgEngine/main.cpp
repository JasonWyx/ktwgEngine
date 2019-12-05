#include "application.h"

int main()
{
  Application::Initialize();
  Application::GetInstance().Run();
  Application::Shutdown();
  return 0;
}