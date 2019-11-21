#ifndef _D3D11_RESOURCE_H_
#define _D3D11_RESOURCE_H_

#include <d3d11.h>
#include "d3d11defines.h"

class D3D11PipelineResource
{
};

class D3D11SR : public D3D11PipelineResource
{
public:
  void Init(const ComPtr<ID3D11Resource>& resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc);
  const ComPtr<ID3D11ShaderResourceView>& GetResource() const { return m_Resource; }
private:
  ComPtr<ID3D11ShaderResourceView> m_Resource;
};

class D3D11DS : public D3D11PipelineResource
{
public:
  void Init(const ComPtr<ID3D11Resource>& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);
  const ComPtr<ID3D11DepthStencilView>& GetResource() const { return m_Resource; }
private:
  ComPtr<ID3D11DepthStencilView> m_Resource;
};

class D3D11StaticPipelineResourceFactory
{
  static D3D11PipelineResource* CreateNewSR()
  {
    return new D3D11SR{};
  }

  static D3D11PipelineResource* CreateNewDS()
  {
    return new D3D11DS{};
  }
};

#define DEFINE_STATIC_RESOURCE(name, tag) \
struct GFX_STATIC_RESOURCE_WRAPPER_#tag#name \
{\
  GFX_STATIC_RESOURCE_WRAPPER_#tag#name()\
  :m_Resource{D3D11StaticPipelineResourceFactory::CreateNew#tag()}\
  {\
  }\
  ~GFX_STATIC_RESOURCE_WRAPPER_#tag#name()\
  {\
    delete m_Resource;\
  }\
  D3D11#tag* m_Resource;\
}\
static GFX_STATIC_RESOURCE_WRAPPER_#tag#name gfx_static_resource_wrapper_#tag#name;\
D3D11#tag* #name = gfx_static_resource_wrapper_#tag#name.m_Resource;

#define DECLARE_STATIC_RESOURCE(name, tag) \
extern D3D11#tag* #name

#define DEFINE_SR(name) DEFINE_STATIC_RESOURCE(name, SR)

#define DECLARE_SR(name) DECLARE_STATIC_RESOURCE(name, SR)

#define DEFINE_DS(name) DEFINE_STATIC_RESOURCE(name, DS)

#define DECLARE_DS(name) DECLARE_STATIC_RESOURCE(name, DS)

#endif
