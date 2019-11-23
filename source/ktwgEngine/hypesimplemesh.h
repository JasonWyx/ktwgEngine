#pragma once

#include "hypegraphicsobject.h"
#include "d3d11hardwarebuffer.h"
#include "vector3.h"
#include "quaternion.h"

enum SimpleMeshShape
{
  NONE = 0,
  CUBE,
  SPHERE
};

class HypeSimpleMesh : public HypeGraphicsObject
{
  struct Vertex
  {
    Vec3 m_Position;
    Vec3 m_Normal;
  };

public:
  HypeSimpleMesh();

  void SetShape(SimpleMeshShape shape);
  
  void Regenerate();

  void Draw() override;

private:
  static void CreateIndexBuffer(uint32_t stacks, uint32_t slices, std::vector<uint32_t>& indices);
  void ClearResources();
  void CreateCubeResources();
  void CreateSphereResources();
  void InitializeHardwareBuffers(const std::vector<Vec3>& positions, const std::vector<Vec3>& normals, const std::vector<uint32_t>& indices);

  D3D11HardwareBuffer* m_VertexBuffer;
  D3D11HardwareBuffer* m_IndexBuffer;
  SimpleMeshShape m_Shape;
  
  uint32_t m_NumStacks;
  uint32_t m_NumSlices;

  uint32_t m_NumIndices;
};
