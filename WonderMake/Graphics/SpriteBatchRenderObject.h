#pragma once
#include "Graphics/RenderObject.h"

class SpriteBatchRenderObject 
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Origin
		, EVertexAttribute::Scale
		, EVertexAttribute::Color
		, EVertexAttribute::Rotation>
{
public:
	SpriteBatchRenderObject();

	u32 AddSprite();

	void SetPosition(const i32 aIndex, const SVector2f aPosition);
	void SetColor(const i32 aIndex, const SColor aColor);
	void SetScale(const i32 aIndex, const SVector2f aScale);
	void SetScale(const i32 aIndex, const f32 aScale);
	void SetOrigin(const i32 aIndex, const SVector2f aOrigin);
	void SetTexture(ResourceProxy<Texture> aResource);

	SVector2f GetTextureSize() const;
};

