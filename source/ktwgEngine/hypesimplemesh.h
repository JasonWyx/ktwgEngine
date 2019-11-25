#pragma once

#include "hypegraphicobject.h"
#include "d3d11hardwarebuffer.h"
#include "vector3.h"
#include "quaternion.h"

enum SimpleMeshShape
{
  NONE = 0,
  CUBE,
  SPHERE
};

class Entity;

class HypeSimpleMeshInstance : public HypeGraphicObjectInstance
{
public:
  HypeSimpleMeshInstance(Entity& owner);

  Matrix4 GetWorldTransform() const;
};

class HypeSimpleMesh : public HypeGraphicObject
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

  void DrawInstances() override;

  void SetNumStacks(uint32_t numStacks) { m_NumStacks = numStacks; }
  void SetNumSlices(uint32_t numSlices) { m_NumSlices = numSlices; }

private:
  static void CreateIndexBuffer(uint32_t stacks, uint32_t slices, std::vector<uint32_t>& indices);

  void ClearResources         ();
  void CreateCubeResources    ();
  void CreateSphereResources  ();

  void InitializeHardwareBuffers(const std::vector<Vec3>& positions, const std::vector<Vec3>& normals, const std::vector<uint32_t>& indices);
  
  HypeGraphicObjectInstance * NotifyInstanceCreatedInternal(Entity & instance) override;

  D3D11HardwareBuffer* m_VertexBuffer;
  D3D11HardwareBuffer* m_IndexBuffer;
  SimpleMeshShape m_Shape;
  
  uint32_t m_NumStacks;
  uint32_t m_NumSlices;

  uint32_t m_NumIndices;
};
