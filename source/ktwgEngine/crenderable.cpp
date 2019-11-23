#include "crenderable.h"

CRenderable::CRenderable(Entity & entity, uint32_t id)
:Component{entity, id}, m_GraphicObject{nullptr}
{
}
