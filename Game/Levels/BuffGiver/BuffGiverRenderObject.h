#pragma once
#include "Graphics/RenderObject.h"
class BuffGiverRenderObject : public RenderObject<EVertexAttribute::Position, EVertexAttribute::Scale, EVertexAttribute::Color>
{
public:
	BuffGiverRenderObject();
};

