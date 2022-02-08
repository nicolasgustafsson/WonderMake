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
	SpriteRenderObject(const std::string_view aTextureAssetLink);

	void SetColor(const SColor aColor);
};

