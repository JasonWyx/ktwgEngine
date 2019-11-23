#pragma once

class HypeGraphicObjectInstance
{
public:
  virtual void Draw() = 0;
};

class HypeGraphicObject
{
public:
  virtual void DrawInstances() = 0;

  // Todo instances?
};
