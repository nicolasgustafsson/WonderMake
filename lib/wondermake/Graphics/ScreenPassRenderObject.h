#pragma once
#include "RenderObject.h"
#include "RenderTarget.h"

class ScreenPassRenderObject : public RenderObject
	<EVertexAttribute::Position,
	EVertexAttribute::TexCoord>
{
public:
	ScreenPassRenderObject(ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem, ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem, ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem, const std::filesystem::path& aFragmentShader);
	~ScreenPassRenderObject();
};

