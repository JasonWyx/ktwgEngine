#include "hypesimplemesh.h"
#include "d3d11renderapi.h"
#include "d3d11device.h"

HypeSimpleMesh::HypeSimpleMesh()
:m_VertexBuffer{nullptr}, m_IndexBuffer{nullptr}, m_Shape{NONE}
{
}

void HypeSimpleMesh::SetShape(SimpleMeshShape shape)
{
  if (shape != m_Shape)
  {
    m_Shape = shape;
    Regenerate();
  }
}

void HypeSimpleMesh::Regenerate()
{
  switch (m_Shape)
  {
  case CUBE:
    ClearResources();
    CreateCubeResources();
    break;
  case SPHERE:
    ClearResources();
    CreateSphereResources();
    break;
  case NONE:
    ClearResources();
    break;
  }
}

void HypeSimpleMesh::DrawInstances()
{
  D3D11Context& context = D3D11RenderAPI::GetInstance().GetDevice()->GetImmediateContext();
  context.AddVertexBuffer(m_VertexBuffer, sizeof(Vertex), 0);
  context.AddVertexBuffer(m_VertexBuffer, sizeof(Vertex), sizeof(Vec3));
  context.FlushVertexBuffers();

  context.SetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

  context.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_NumIndices, 0U, 0);
}

void HypeSimpleMesh::ClearResources()
{
  delete m_VertexBuffer;
  delete m_IndexBuffer;
}

void HypeSimpleMesh::CreateIndexBuffer(uint32_t stacks, uint32_t slices, std::vector<uint32_t>& indices)
{
  uint32_t i0 = 0, i1 = 0, i2 = 0;
  uint32_t stride = slices + 1;

  for (uint32_t i = 0; i < stacks; ++i)
  {
    uint32_t rowStart = i * stride;

    for (uint32_t j = 0; j < slices; ++j)
    {

      i0 = rowStart + j;
      i1 = i0 + stride;
      i2 = i0 + 1;

      /*  Add the indices for the first triangle */
      indices.emplace_back(i0);
      indices.emplace_back(i1);
      indices.emplace_back(i2);

      i2 = i0 + 1;
      i0 = i1;
      i1 = i0 + 1;

      indices.emplace_back(i0);
      indices.emplace_back(i1);
      indices.emplace_back(i2);
    }
  }
}

void HypeSimpleMesh::CreateCubeResources()
{
  std::vector<Vec3> positions;
  std::vector<Vec3> normals;
  std::vector<uint32_t> indices;

  std::vector<Vec3> planePositions;
  std::vector<uint32_t> planeIndices;

  for (auto stack = 0U; stack <= m_NumStacks; ++stack)
  {
    float row = (float)stack / m_NumStacks;

    for (auto slice = 0U; slice <= m_NumSlices; ++slice)
    {
      float col = (float)slice / m_NumSlices;

      auto pos = Vec3(col - 0.5f, 0.5f - row, 0.0f);
      planePositions.emplace_back(pos);
    }
  }

  CreateIndexBuffer(m_NumStacks, m_NumSlices, planeIndices);

  /*  Use similar method to A1 to create the cube from 3 planes above */
  Vec3 const translateArray[] =
  {
    Vec3(+0.0f, +0.0f, +0.5f), // Z+
    Vec3(+0.0f, +0.0f, -0.5f), // Z-
    Vec3(+0.5f, +0.0f, +0.0f), // X+
    Vec3(-0.5f, +0.0f, +0.0f), // X-
    Vec3(+0.0f, +0.5f, +0.0f), // Y+
    Vec3(+0.0f, -0.5f, +0.0f), // Y-
  };

  // Rotation quaternions
  Quaternion const rotateArray[] =
  {
    Quaternion{},                                                 // Z+
    ConvertAxisAngleToQuaternion(Vec3{ 0.f, 1.f, 0.f }, +180.0f), // Z-
    ConvertAxisAngleToQuaternion(Vec3{ 0.f, 1.f, 0.f }, +90.f),   // X+       
    ConvertAxisAngleToQuaternion(Vec3{ 0.f, 1.f, 0.f }, -90.f),   // X-
    ConvertAxisAngleToQuaternion(Vec3{ 1.f, 0.f, 0.f }, -90.f),   // Y+
    ConvertAxisAngleToQuaternion(Vec3{ 1.f, 0.f, 0.f }, +90.f)    // Y-
  };

  uint32_t planeNumVertices = static_cast<uint32_t>(planePositions.size());
  uint32_t planeNumIndices = static_cast<uint32_t>(planeIndices.size());

  // Generate the 6 planes of the cube
  for (auto i = 0; i < 6; ++i)
  {
    // Base number of vertices to add to each index
    uint32_t numVertices = static_cast<uint32_t>(positions.size());

    // Generate vertices
    for (auto j = 0U; j < planeNumVertices; ++j)
    {
      Vec3 pos = planePositions[j];
      rotateArray[i].Rotate(pos);
      pos += translateArray[i];
      Vec3 nrm = Vec3{0.0f, 0.0f, 1.0f};
      rotateArray[i].Rotate(nrm);

      positions.emplace_back(pos);
      normals.emplace_back(nrm);
    }

    // Generate indices
    for (auto j = 0U; j < planeNumIndices; ++j)
    {
      indices.emplace_back(planeIndices[j] + numVertices);
    }
  }
  InitializeHardwareBuffers(positions, normals, indices);
}

void HypeSimpleMesh::CreateSphereResources()
{
  std::vector<Vec3> positions;
  std::vector<Vec3> normals;
  std::vector<uint32_t> indices;
  for (auto stack = 0U; stack <= m_NumStacks; ++stack)
  {
    float row = (float)stack / m_NumStacks;

    for (auto slice = 0U; slice <= m_NumSlices; ++slice)
    {
      float col = (float)slice / m_NumSlices;

      float alpha = col * 2.f * PI;
      float beta = (0.5f - row) * PI;

      float cosAlpha = cosf(alpha);
      float sinAlpha = sinf(alpha);
      float cosBeta = cosf(beta);
      float sinBeta = sinf(beta);

      float x = cosBeta * sinAlpha;
      float y = sinBeta;
      float z = cosBeta * cosAlpha;

      auto pos = Vec3{ 0.5f * x, 0.5f * y, 0.5f * z };
      auto nrm = Vec3(x, y, z);
      // dP/dA
      auto tan = Vec3(z, 0.f, -x);
      // dP/dB
      auto bitan = Vec3(-sinBeta * sinAlpha, cosBeta, -sinBeta * cosAlpha);

      positions.emplace_back(pos);
      normals.emplace_back(nrm);
    }
  }

  CreateIndexBuffer(m_NumStacks, m_NumSlices, indices);
  InitializeHardwareBuffers(positions, normals, indices);
}

void HypeSimpleMesh::InitializeHardwareBuffers(const std::vector<Vec3>& positions, const std::vector<Vec3>& normals, const std::vector<uint32_t>& indices)
{
  uint32_t numVertices = (uint32_t)positions.size();
  std::vector<Vertex> vertices;
  vertices.resize(numVertices);
  
  for (uint32_t i = 0U; i < numVertices; ++i)
  {
    vertices[i].m_Position = positions[i];
    vertices[i].m_Normal = normals[i];
  }

  uint32_t numIndices = (uint32_t)indices.size();
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  m_VertexBuffer = new D3D11HardwareBuffer{device, D3D11_BT_VERTEX, D3D11_USAGE_DEFAULT, numVertices, sizeof(Vertex), false, false, false, false};
  m_IndexBuffer = new D3D11HardwareBuffer{device, D3D11_BT_INDEX, D3D11_USAGE_DEFAULT, numIndices, sizeof(uint32_t), false, false, false, false};

  m_NumIndices = numIndices;
}
