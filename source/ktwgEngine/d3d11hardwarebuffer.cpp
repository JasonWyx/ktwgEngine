#include "d3d11hardwarebuffer.h"
#include "d3d11device.h"
#include <iostream>

D3D11HardwareBuffer::D3D11HardwareBuffer(D3D11Device * device, D3D11_BUFFER_TYPE type, D3D11_USAGE resourceUsage, uint32_t elementCount, uint32_t elementSize, bool unorderedGpuAccess, bool streamOut, bool shaderUsage, bool drawIndirect)
:m_BufferSize{elementCount * elementSize}, m_Buffer{nullptr}, m_StagingBuffer{nullptr}, m_Flags{}, m_ElementCount{elementCount}, m_ElementSize{elementSize}
{
  m_Flags.m_BufferType                = (uint8_t)type;
  m_Flags.m_StagingUploadRequired     = false;
  m_Flags.m_UnorderedGpuAccess        = unorderedGpuAccess;
  m_Flags.m_UseTemporaryStagingBuffer = false;
  m_Flags.m_StreamOut                 = streamOut;

  if (resourceUsage == D3D11_USAGE_STAGING)
  {
    m_Desc.Usage = D3D11_USAGE_STAGING;
    m_Desc.BindFlags = 0;
    m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  }
  else
  {
    m_Desc.Usage = resourceUsage;
    m_Desc.CPUAccessFlags = resourceUsage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
    
    switch (type)
    {
    case D3D11_BT_VERTEX:
      m_Desc.BindFlags = streamOut ? D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_VERTEX_BUFFER;
      break;
    case D3D11_BT_INDEX:
      m_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      break;
    case D3D11_BT_CONSTANT:
      m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      break;
    case D3D11_BT_STRUCTURED:
      m_Desc.BindFlags = unorderedGpuAccess ? D3D11_BIND_UNORDERED_ACCESS : 0;
      m_Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
      m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
      m_Desc.StructureByteStride = elementSize;
      break;
    case D3D11_BT_RAW:
      m_Desc.BindFlags = unorderedGpuAccess ? D3D11_BIND_UNORDERED_ACCESS : 0;
      m_Desc.BindFlags |= shaderUsage ? D3D11_BIND_SHADER_RESOURCE : 0;
      m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
      m_Desc.MiscFlags |= drawIndirect ? D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS : 0;
      break;
    }
  }
  
  HRESULT hr = device->GetDevice()->CreateBuffer(&m_Desc, nullptr, &m_Buffer);

  if (FAILED(hr))
  {
    auto msg = device->GetErrors();
    std::cerr << msg << "\n";
  }
}

void D3D11HardwareBuffer::Read(uint32_t offset, uint32_t length, void * dest)
{
}

void D3D11HardwareBuffer::Write(uint32_t offset, uint32_t length, const void * src, GPU_WRITE_TYPE writeType)
{
}

void D3D11HardwareBuffer::Copy(D3D11HardwareBuffer * src, uint32_t srcOffset, uint32_t dstOffset, uint32_t length, bool discardEntireBuffer)
{
}

void * D3D11HardwareBuffer::Map(uint32_t offset, uint32_t length, GPU_LOCK_OPTIONS lockOptions)
{
  return nullptr;
}

void D3D11HardwareBuffer::Unmap()
{
}
