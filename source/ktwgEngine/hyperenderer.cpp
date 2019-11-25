#include "hyperenderer.h"

#include "Shaders/cpp/ShaderCommon.h"
#include "d3d11shader.h"
#include "d3d11staticresources.h"
#include "hypegraphicsworld.h"
#include "hyperendernode.h"
#include "hypecamera.h"
#include "d3d11renderwindowmanager.h"
#include "d3d11renderwindow.h"
#include "matrix4.h"
#include "hypesimpleforwardrendernode.h"

#include "Shaders/cpp/SimpleForwardParams.h"

#include <fstream>

DECLARE_VS(SimpleForwardVS);
DECLARE_PS(SimpleForwardPS);
DEFINE_STATIC_BUFFER(GeometryConstantBuffer);
DECLARE_STATIC_TEXTURE(FinalColorOutput);
DECLARE_STATIC_TEXTURE(MainRenderDepthStencil);

void HypeRenderer::LoadSimpleForward()
{
  std::fstream f{ "Shaders/hlsl/SimpleForward.hlsl" };

  SHADER_DESC desc;
  desc.m_Entry = "Shade_Vertex";
  desc.m_Source = std::string{ std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{} };
  desc.m_Type = VS;
  SimpleForwardVS = CREATE_VS(desc);
  InputLayoutKey inputLayoutKey;
  inputLayoutKey.AddInputElement(POSITION, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 0);
  inputLayoutKey.AddInputElement(NORMAL, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 0);
  SimpleForwardVS->SetInputLayout(inputLayoutKey);
  desc.m_Entry = "Shade_Pixel";
  desc.m_Type = PS;
  SimpleForwardPS = CREATE_PS(desc);
}

void HypeRenderer::UnloadSimpleForward()
{
}

void HypeRenderer::InitializeInternal()
{
  HypeGraphicsWorld::Initialize();
  SetupRenderNodes();

  RENDER_WINDOW_DESC desc;
  desc.m_VideoMode = { 1280, 600, 60.f };
  desc.m_Title = "TestWindow";
  desc.m_Fullscreen = false;
  desc.m_Hidden = false;
  desc.m_ShowBorder = true;
  desc.m_ShowTitleBar = true;
  desc.m_Left = -1;
  desc.m_Top = -1;
  desc.m_MultisampleCount = 1;

  m_RenderWindow = D3D11RenderWindowManager::GetInstance().CreatePrimaryRenderWindow(desc);
  m_RenderWindow->Init();
  m_RenderWindow->SetHidden(false);

  // Initialize Shader Inputs
  ShaderInputs::SimpleForwardParams::InitializeHWResources();
}

void HypeRenderer::ShutdownInternal()
{
  HypeGraphicsWorld::Shutdown();
  delete m_RenderWindow;
}

void HypeRenderer::SetupRenderNodes()
{
  #define DECLARE_RENDER_NODE(name, ...) m_RenderNodes.emplace_back(new name{__VA_ARGS__})

  DECLARE_RENDER_NODE(HypeSimpleForwardRenderNode);

  #undef DECLARE_RENDER_NODE
}

void HypeRenderer::LoadSystemShaders()
{
  LoadSimpleForward();
}

void HypeRenderer::UnloadSystemShaders()
{
  UnloadSimpleForward();
}

void HypeRenderer::CreateCommonResources()
{
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  GET_STATIC_RESOURCE(GeometryConstantBuffer) = new D3D11HardwareBuffer{device, D3D11_BT_CONSTANT, D3D11_USAGE_DYNAMIC, 1, sizeof(Matrix4), false, false, true, false};
  device->GetImmediateContext().AddConstantBuffer<VS>(GET_STATIC_RESOURCE(GeometryConstantBuffer));
  device->GetImmediateContext().AddConstantBuffer<PS>(GET_STATIC_RESOURCE(GeometryConstantBuffer));
  device->GetImmediateContext().FlushConstantBuffers(GeometryConstantBufferSlot);
}

void HypeRenderer::Update()
{
  D3D11Device* device = D3D11RenderAPI::GetInstance().GetDevice();
  Camera* view = HypeGraphicsWorld::GetInstance().GetView();
  if(view)
  {
    view->Update();
    Matrix4 viewProj = view->GetViewMatrix() * view->GetProjectionMatrix();
    device->GetImmediateContext().Set(view->GetViewMatrix(), view->GetProjectionMatrix(), viewProj);
  }

  device->GetImmediateContext().ClearRTV(GET_STATIC_RESOURCE(FinalColorOutput), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0.0f, 0.0f, 0.0f, 1.0f);
  device->GetImmediateContext().ClearDSV(GET_STATIC_RESOURCE(MainRenderDepthStencil), DXGI_FORMAT_D32_FLOAT, D3D11_CLEAR_DEPTH, 1.0f, 0);

  for (auto& elem : m_RenderNodes)
  {
    elem->Setup(*device);
    elem->Render(*device);
  }

  m_RenderWindow->SwapBuffers();
}
