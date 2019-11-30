// VS Shade_Vertex
// PS Shade_Pixel

#define _GEOMETRY_PASS_
#define _SIMPLE_FORWARD_

#include "Shaders/hlsl/ShaderCommon.h"
#include "Shaders/hlsl/SimpleForwardParams.h"

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
  output.m_SVPosition = mul(float4(input.m_Position, 1.0), g_ModelViewProjection);
  float3 worldNormal = mul(input.m_Normal, (float3x3)g_WorldInverseTranspose);
  float3 lightDir = float3(1.0, 1.0, -1.0);
  lightDir = normalize(lightDir);
  output.m_Color = saturate(0.2f + dot(worldNormal, lightDir)) * g_SimpleForwardParamsColor;
  return output;
}

float4 Shade_Pixel(in PS_INPUT input) : SV_TARGET0
{
  return input.m_Color;
}
