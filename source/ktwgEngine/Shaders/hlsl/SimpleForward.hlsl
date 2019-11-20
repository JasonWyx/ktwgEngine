// VS Shade_Vertex
// PS Shade_Pixel

#define _GEOMETRY_PASS_

#include "Shaders/hlsl/ShaderCommon.h"

struct VS_INPUT
{
  float4 m_Position : POSITION;
  float4 m_Color    : COLOR;
};

struct PS_INPUT
{
  float4 m_SVPosition : SV_POSITION;
  float4 m_Color          : COLOR;  
};

PS_INPUT Shade_Vertex(in VS_INPUT input)
{
  PS_INPUT output;
  output.m_SVPosition = mul(input.m_Position, g_ModelViewProjection);
  output.m_Color = input.m_Color;
  return output;
}

float4 Shade_Pixel(in PS_INPUT input) : SV_TARGET0
{
  return input.m_Color;
}
