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

	void SetPosition(const SVector2f aPosition);
	void SetColor(const SColor aColor);
	void SetScale(const SVector2f aScale);
	void SetScale(const f32 aScale);
};

