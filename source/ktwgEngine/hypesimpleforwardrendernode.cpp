#include "hypesimpleforwardrendernode.h"
#include "d3d11staticresources.h"
#include "d3d11device.h"
#include "d3d11shader.h"

#include "hypegraphicsworld.h"
#include "hypegraphicobject.h"

#include "Shaders/cpp/ShaderCommon.h"

DECLARE_VS(SimpleForwardVS);
DECLARE_PS(SimpleForwardPS);

DECLARE_STATIC_TEXTURE(FinalColorOutput);
DECLARE_STATIC_TEXTURE(MainRenderDepthStencil);

void HypeSimpleForwardRenderNode::Setup(D3D11Device& device)
{
  device.GetImmediateContext().Set(SimpleForwardVS);
  device.GetImmediateContext().Set(SimpleForwardPS);
  device.GetImmediateContext().AddRenderTarget(GET_STATIC_RESOURCE(FinalColorOutput), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
  device.GetImmediateContext().SetDepthStencil(GET_STATIC_RESOURCE(MainRenderDepthStencil), DXGI_FORMAT_D32_FLOAT);
  device.GetImmediateContext().FlushRenderTargets();
}

void HypeSimpleForwardRenderNode::Render(D3D11Device& device)
{
  const std::vector<HypeGraphicObject*>& graphicObjects = HypeGraphicsWorld::GetInstance().GetGraphicObjects();

  for (HypeGraphicObject* graphicObject : graphicObjects)
  {
    graphicObject->DrawInstances();
  }
}
