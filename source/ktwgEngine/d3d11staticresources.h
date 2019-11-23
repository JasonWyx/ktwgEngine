#pragma once

#include "d3d11hardwarebuffer.h"
#include "d3d11texture.h"

struct GFX_STATIC_RESOURCE_WRAPPER_Texture
{
  GFX_STATIC_RESOURCE_WRAPPER_Texture()
    :m_Resource{ nullptr }
  {
  }
  ~GFX_STATIC_RESOURCE_WRAPPER_Texture()
  {
    delete m_Resource;
  }
  D3D11Texture* m_Resource;
};

struct GFX_STATIC_RESOURCE_WRAPPER_Buffer
{
  GFX_STATIC_RESOURCE_WRAPPER_Buffer()
    :m_Resource{ nullptr }
  {
  }
  ~GFX_STATIC_RESOURCE_WRAPPER_Buffer()
  {
    delete m_Resource;
  }
  D3D11HardwareBuffer* m_Resource;
};

#define DEFINE_STATIC_RESOURCE(name, type) GFX_STATIC_RESOURCE_WRAPPER_##type name

#define DECLARE_STATIC_RESOURCE(name, type) extern GFX_STATIC_RESOURCE_WRAPPER_##type name

#define DEFINE_STATIC_TEXTURE(name) DEFINE_STATIC_RESOURCE(name, Texture)

#define DEFINE_STATIC_BUFFER(name) DEFINE_STATIC_RESOURCE(name, Buffer)

#define DECLARE_STATIC_TEXTURE(name) DECLARE_STATIC_RESOURCE(name, Texture)

#define DECLARE_STATIC_BUFFER(name) DECLARE_STATIC_RESOURCE(name, Buffer)

#define GET_STATIC_RESOURCE(name) name.m_Resource

#define RESET_STATIC_RESOURCE(name) delete name.m_Resource; name.m_Resource = nullptr
