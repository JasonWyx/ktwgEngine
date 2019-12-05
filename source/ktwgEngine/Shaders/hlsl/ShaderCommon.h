#ifndef _SHADER_COMMON_H_
#define _SHADER_COMMON_H_

// Common Constant Buffers

struct GeometryConstantBuffer
{
  float4x4 m_ModelViewProjection;
  float4x4 m_WorldInverseTranspose;
};

#define GeometryConstantBufferSlot register(b10)

#ifdef _GEOMETRY_PASS_

cbuffer GEOMETRY_CONSTANT_BUFFER : GeometryConstantBufferSlot
{
  GeometryConstantBuffer cGeometryConstantBuffer;
};

float4x4 GetModelViewProjection()
{
  return cGeometryConstantBuffer.m_ModelViewProjection;
}

float4x4 GetWorldInverseTranspose()
{
  return cGeometryConstantBuffer.m_WorldInverseTranspose;
}

#define g_ModelViewProjection GetModelViewProjection()
#define g_WorldInverseTranspose GetWorldInverseTranspose()

#endif // #ifdef _GEOMETRY_PASS_

#endif // #ifndef _SHADER_COMMON_H_