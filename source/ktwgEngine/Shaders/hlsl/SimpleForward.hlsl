// VS Shade_Vertex
// PS Shade_Pixel

#define _GEOMETRY_PASS_

#include "Shaders/hlsl/ShaderCommon.h"

struct VS_INPUT
{
  float3 m_Position : POSITION;
  float3 m_Normal   : NORMAL;
};

struct PS_INPUT
{
  float4 m_SVPosition : SV_POSITION;
  float4 m_Color      : COLOR;  
};

PS_INPUT Shade_Vertex(in VS_INPUT input)
{
  PS_INPUT output;
  output.m_SVPosition = mul(input.m_Position, g_ModelViewProjection);
  output.m_Color = float4(1.0, 1.0, 1.0, 1.0);
  return output;
}

float4 Shade_Pixel(in PS_INPUT input) : SV_TARGET0
{
  return input.m_Color;
}
