#pragma once

class HypeMaterial
{
public:
  
  HypeMaterial();
  HypeMaterial(float r, float g, float b, float a);

  void SetColor(float r, float g, float b, float a);
  void GetColor(float& r, float& g, float& b, float& a);

private:
  float   m_Color[4];
};
