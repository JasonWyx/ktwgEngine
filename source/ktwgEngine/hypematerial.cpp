#include "hypematerial.h"

HypeMaterial::HypeMaterial()
: m_Color{1.0f, 1.0f, 1.0f, 1.0f}
{
}

HypeMaterial::HypeMaterial(float r, float g, float b, float a)
:m_Color{r, g, b, a}
{
}

void HypeMaterial::SetColor(float r, float g, float b, float a)
{
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
  m_Color[3] = a;
}

void HypeMaterial::GetColor(float & r, float & g, float & b, float & a)
{
  r = m_Color[0];
  g = m_Color[1];
  b = m_Color[2];
  a = m_Color[3];
}
