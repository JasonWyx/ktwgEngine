#include "hypegraphicsworld.h"
#include "hypegraphicobject.h"
#include "entity.h"
#include "hypecamera.h"
#include "hypesimplemesh.h"

void HypeGraphicsWorld::InitializeInternal()
{
  m_View = nullptr;

  HypeSimpleMesh* simpleMesh = new HypeSimpleMesh{};
  simpleMesh->SetNumSlices(2);
  simpleMesh->SetNumStacks(2);
  simpleMesh->SetName("Cube");
  simpleMesh->SetShape(CUBE);
  simpleMesh->Regenerate();
  AddGraphicObject(simpleMesh);
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

HypeGraphicObject * HypeGraphicsWorld::GetGraphicObject(const std::string & name)
{
  auto it = std::find_if(m_GraphicObjects.begin(), m_GraphicObjects.end(), [&name](HypeGraphicObject* obj) { return obj->GetName() == name; });
  if(it != m_GraphicObjects.end())
    return *it;
  return nullptr;
}
