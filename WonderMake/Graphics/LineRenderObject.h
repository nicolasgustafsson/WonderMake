#pragma once
#include "RenderObject.h"

class LineRenderObject
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Color>
{
public:
	LineRenderObject();
};

