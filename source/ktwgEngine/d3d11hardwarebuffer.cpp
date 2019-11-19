#include "d3d11hardwarebuffer.h"
#include "d3d11device.h"
#include "d3d11renderapi.h"
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
  void* src = Map(offset, length, LO_READ_ONLY);
  memcpy(dest, src, length);
  Unmap();
}

void D3D11HardwareBuffer::Write(uint32_t offset, uint32_t length, const void * src, GPU_WRITE_TYPE writeType)
{
  if (m_Desc.Usage == D3D11_USAGE_DYNAMIC || m_Desc.Usage == D3D11_USAGE_STAGING)
  {
    GPU_LOCK_OPTIONS options = LO_WRITE_ONLY;
    if (writeType == WT_DISCARD)
    {
      options = LO_WRITE_ONLY_DISCARD;
    }
    else if (writeType == WT_NO_OVERWRITE)
    {
      options = LO_WRITE_ONLY_NO_OVERWRITE;
    }
    void* dest = Map(offset, length, options);
    memcpy(dest, src, length);
    Unmap();
  }
  else if (m_Desc.Usage == D3D11_USAGE_DEFAULT)
  {
    D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();

    if (m_Flags.m_BufferType == D3D11_BT_CONSTANT)
    {
      device->GetImmediateContext()->UpdateSubresource(m_Buffer.Get(), 0, nullptr, src, 0, 0);
    }
    else
    {
      D3D11_BOX dstBox;
      dstBox.left = offset;
      dstBox.right = offset + length;
      dstBox.top = 0;
      dstBox.bottom = 1;
      dstBox.front = 0;
      dstBox.back = 1;

      device->GetImmediateContext()->UpdateSubresource(m_Buffer.Get(), 0, &dstBox, src, 0, 0);

      if (device->HasError())
      {
        auto err = device->GetErrors();
        std::cerr << err << std::endl;
      }
    }
  }
}

void D3D11HardwareBuffer::Copy(D3D11HardwareBuffer * src, uint32_t srcOffset, uint32_t dstOffset, uint32_t length, bool discardEntireBuffer)
{
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  // If copying two same sized buffers
  if (srcOffset == 0 && dstOffset == 0 && length == m_BufferSize && length == src->m_BufferSize)
  {
    device->GetImmediateContext()->CopyResource(m_Buffer.Get(), static_cast<D3D11HardwareBuffer*>(src)->m_Buffer.Get());
  }
  else
  {
    D3D11_BOX srcBox;
    srcBox.left = srcOffset;
    srcBox.right = srcOffset + length;
    srcBox.top = 0;
    srcBox.bottom = 1;
    srcBox.front = 0;
    srcBox.back = 1;

    device->GetImmediateContext()->CopySubresourceRegion(m_Buffer.Get(), 0, dstOffset, 0, 0, static_cast<D3D11HardwareBuffer*>(src)->m_Buffer.Get(), 0, &srcBox);
  }
}

void * D3D11HardwareBuffer::Map(uint32_t offset, uint32_t length, GPU_LOCK_OPTIONS lockOptions)
{
  if ((m_Desc.Usage == D3D11_USAGE_DYNAMIC && lockOptions != LO_READ_ONLY) || m_Desc.Usage == D3D11_USAGE_STAGING)
  {
    D3D11_MAP mapType{};
    
    switch (lockOptions)
    {
    case LO_WRITE_ONLY_DISCARD:
      if (m_Desc.Usage & D3D11_USAGE_DYNAMIC)
      {
        mapType = D3D11_MAP_WRITE_DISCARD;
      }
      else
      {
        mapType = D3D11_MAP_WRITE;
      }
      break;
    case LO_WRITE_ONLY_NO_OVERWRITE:
      if (m_Flags.m_BufferType == D3D11_BT_VERTEX || m_Flags.m_BufferType == D3D11_BT_INDEX)
      {
        mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
      }
      else
      {
        mapType = D3D11_MAP_WRITE;
      }
      break;
    case LO_WRITE_ONLY:
      mapType = D3D11_MAP_WRITE;
      break;
    case LO_READ_WRITE:
      if ((m_Desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) != 0 && (m_Desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE & D3D11_CPU_ACCESS_WRITE) != 0)
      {
        mapType = D3D11_MAP_READ_WRITE;
      }
      else if ((m_Desc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE) != 0)
      {
        mapType = D3D11_MAP_WRITE;
      }
      else
      {
        mapType = D3D11_MAP_READ;
      }
      break;
    case LO_READ_ONLY:
      mapType = D3D11_MAP_READ;
      break;
    }

    D3D11_MAPPED_SUBRESOURCE subResource;
    subResource.pData = nullptr;

    D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
    HRESULT hr = device->GetImmediateContext()->Map(m_Buffer.Get(), 0, mapType, 0, &subResource);
    if (FAILED(hr))
    {
      auto msg = device->GetErrors();
      std::cerr << msg << std::endl;
      return nullptr;
    }

    return (void*)((char*)subResource.pData + offset);
  }
  else
  {
    D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
    m_Flags.m_UseTemporaryStagingBuffer = true;
    
    if (m_StagingBuffer == nullptr)
    {
      m_StagingBuffer = new D3D11HardwareBuffer(device, (D3D11_BUFFER_TYPE)m_Flags.m_BufferType, m_Desc.Usage, m_ElementCount, m_ElementSize, (bool)m_Flags.m_UnorderedGpuAccess, (bool)m_Flags.m_StreamOut, false, false);
    }

    if (lockOptions == LO_READ_ONLY || lockOptions == LO_READ_WRITE)
    {
      m_StagingBuffer->Copy(this, 0, 0, m_BufferSize, true);
    }

    m_Flags.m_StagingUploadRequired = lockOptions != LO_READ_ONLY;
    return m_StagingBuffer->Map(offset, length, lockOptions);
  }
}

void D3D11HardwareBuffer::Unmap()
{
  if (m_Flags.m_UseTemporaryStagingBuffer)
  {
    m_Flags.m_UseTemporaryStagingBuffer = false;
    m_StagingBuffer->Unmap();
    if (m_Flags.m_StagingUploadRequired)
    {
      Copy(m_StagingBuffer, 0, 0, m_BufferSize, true);
    }
    delete m_StagingBuffer;
    m_StagingBuffer = nullptr;
  }
  else
  {
    D3D11RenderAPI::GetInstance().GetDevice()->GetImmediateContext()->Unmap(m_Buffer.Get(), 0);
  }
}
