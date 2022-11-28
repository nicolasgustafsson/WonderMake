#pragma once

#include "Graphics/RenderObject.h"

class SpriteRenderObject 
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Origin
		, EVertexAttribute::Scale
		, EVertexAttribute::Color
		, EVertexAttribute::Rotation>
{
public:
	SpriteRenderObject(
		ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem,
		ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem,
		ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem,
		ResourceProxy<Texture> aTextureAsset);

	void SetColor(const SColor aColor);
};

