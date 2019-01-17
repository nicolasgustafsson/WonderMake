#pragma once
#include "RenderObject.h"
#include "RenderTarget.h"

class ScreenPassRenderObject : public RenderObject
	<EVertexAttribute::Position,
	EVertexAttribute::TexCoord>
{
public:
	ScreenPassRenderObject(const std::filesystem::path& aFragmentShader);
	~ScreenPassRenderObject();
};

