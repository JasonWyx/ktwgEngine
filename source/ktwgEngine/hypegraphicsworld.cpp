#include "hypegraphicsworld.h"
#include "hypegraphicobject.h"
#include "entity.h"
#include "hypecamera.h"

void HypeGraphicsWorld::InitializeInternal()
{
  m_View = nullptr;
}

void HypeGraphicsWorld::ShutdownInternal()
{
  for (auto& gfxObj : m_GraphicObjects)
  {
    delete gfxObj;
  }

  delete m_View;
}

Camera* HypeGraphicsWorld::NotifyViewCreated(Entity& entity)
{
  if (m_View)
  {
    delete m_View;
  }
  m_View = new Camera{entity};
  return m_View;
}

void HypeGraphicsWorld::NotifyViewDestroyed()
{
  delete m_View;
  m_View = nullptr;
}
