#pragma once

namespace ShaderInputs
{

class SimpleForwardParams
{
public:
  
  static void InitializeHWResources();

  void SetColor(float r, float g, float b, float a);
  void GetColor(float& r, float& g, float& b, float& a);

  void Set();

private:
  float m_Color[4];
};

#define SimpleForwardParamsSlot 0

} // namespace ShaderInputs