#include "pch.h"
#include "SpriteBatchRenderObject.h"

SpriteBatchRenderObject::SpriteBatchRenderObject()
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/Sprite.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/Sprite.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/Sprite.frag"
		,	""
		,	0  })
{
}

u32 SpriteBatchRenderObject::AddSprite()
{
	IncreaseRenderCount();
	SetAttribute<EVertexAttribute::Scale>(*myRenderCount - 1, SVector2f{ 1.0f, 1.0f });
	SetAttribute<EVertexAttribute::Color>(*myRenderCount - 1, { 1.0f, 1.0f, 1.0f, 1.0f });

	return *myRenderCount - 1;
}

void SpriteBatchRenderObject::SetPosition(const i32 aIndex, const SVector2f aPosition)
{
	SetAttribute<EVertexAttribute::Position>(aIndex, aPosition);
}

void SpriteBatchRenderObject::SetColor(const i32 aIndex, const SColor aColor)
{
	SetAttribute<EVertexAttribute::Color>(aIndex, aColor);
}

void SpriteBatchRenderObject::SetScale(const i32 aIndex, const SVector2f aScale)
{
	SetAttribute<EVertexAttribute::Scale>(aIndex, aScale);
}

void SpriteBatchRenderObject::SetScale(const i32 aIndex, const f32 aScale)
{
	SetScale(aIndex, { aScale, aScale });
}

void SpriteBatchRenderObject::SetTexture(ResourceProxy<Texture> aResource)
{
	Super::SetTexture(aResource);
	if (myTextures[0].IsValid())
		myShaderProgram.SetProperty("TextureSize", SVector2f{ static_cast<f32>(myTextures[0]->GetWidth()), static_cast<f32>(myTextures[0]->GetHeight()) });
}

SVector2f SpriteBatchRenderObject::GetTextureSize() const
{
	if (!myTextures[0].IsValid())
		return SVector2f::Zero();
	return { myTextures[0]->GetWidth(), myTextures[0]->GetHeight() };
}
