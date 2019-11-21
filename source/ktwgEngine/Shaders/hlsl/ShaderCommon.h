#ifndef _SHADER_COMMON_H_
#define _SHADER_COMMON_H_

// Common Constant Buffers

struct GeometryConstantBuffer
{
  float4x4 m_ModelViewProjection;
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

#define g_ModelViewProjection GetModelViewProjection()

#endif // #ifdef _GEOMETRY_PASS_

#endif // #ifndef _SHADER_COMMON_H_