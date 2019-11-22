#pragma once

#include <d3d11.h>
#include "d3d11defines.h"
#include "d3d11resource.h"

class D3D11Device;

struct HardwareBufferFlags
{
  uint8_t           m_UseTemporaryStagingBuffer : 1;
  uint8_t           m_UnorderedGpuAccess        : 1;
  uint8_t           m_StagingUploadRequired     : 1;
  uint8_t           m_BufferType                : 3;
  uint8_t           m_StreamOut                 : 1;
};

class D3D11HardwareBuffer : public D3D11Resource
{
public:
  D3D11HardwareBuffer(D3D11Device* device, D3D11_BUFFER_TYPE type, D3D11_USAGE resourceUsage, uint32_t elementCount, uint32_t elementSize, bool unorderedGpuAccess, bool streamOut, bool shaderUsage, bool drawIndirect);

  ID3D11Buffer*                 GetBuffer       () const { return static_cast<ID3D11Buffer*>(m_Resource.Get()); }
  const D3D11_BUFFER_DESC&      GetDesc         () const { return m_Desc; }
  uint32_t                      GetElementCount () const { return m_ElementCount; }
  uint32_t                      GetElementSize  () const { return m_ElementSize; }
  uint32_t                      GetBufferSize   () const { return m_BufferSize; }
  HardwareBufferFlags           GetFlags        () const { return m_Flags; }

  void Read     (uint32_t offset, uint32_t length, void* dest);
  void Write    (uint32_t offset, uint32_t length, const void* src, GPU_WRITE_TYPE writeType);
  void Copy     (D3D11HardwareBuffer* src, uint32_t srcOffset, uint32_t dstOffset, uint32_t length, bool discardEntireBuffer = false);

private:
  void* Map(uint32_t offset, uint32_t length, GPU_LOCK_OPTIONS lockOptions);
  void  Unmap();

  D3D11HardwareBuffer* m_StagingBuffer;
  D3D11_BUFFER_DESC    m_Desc;
  uint32_t             m_BufferSize;
  uint32_t             m_ElementCount;
  uint32_t             m_ElementSize;
  HardwareBufferFlags  m_Flags;
};
