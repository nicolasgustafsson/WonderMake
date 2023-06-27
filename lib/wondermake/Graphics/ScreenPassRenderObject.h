#pragma once
#include "RenderObject.h"

#include "wondermake-ui/RenderTarget.h"

class ScreenPassRenderObject : public RenderObject
	<EVertexAttribute::Position,
	EVertexAttribute::TexCoord>
{
public:
	ScreenPassRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram);

};

