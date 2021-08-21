#include "pch.h"
#include "SpriteRenderObject.h"

SpriteRenderObject::SpriteRenderObject(const std::string_view aTextureAssetLink)
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/Sprite.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/Sprite.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/Sprite.frag"
		,	aTextureAssetLink
		,	1  })
{
	if (myTextures[0].IsValid())
		myShaderProgram.SetProperty("TextureSize", SVector2f{ static_cast<f32>(myTextures[0]->GetWidth()), static_cast<f32>(myTextures[0]->GetHeight()) });

	SetAttribute<EVertexAttribute::Scale>(0, SVector2f{ 1.0f, 1.0f });
	SetAttribute<EVertexAttribute::Color>(0, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SpriteRenderObject::SetPosition(const SVector2f aPosition)
{
	SetAttribute<EVertexAttribute::Position>(0, aPosition);
}

void SpriteRenderObject::SetColor(const SColor aColor)
{
	SetAttribute<EVertexAttribute::Color>(0, aColor);
}

void SpriteRenderObject::SetScale(const SVector2f aScale)
{
	SetAttribute<EVertexAttribute::Scale>(0, aScale);
}

void SpriteRenderObject::SetScale(const f32 aScale)
{
	SetScale({ aScale, aScale });
}

SVector2f SpriteRenderObject::GetTextureSize() const
{
	if (!myTextures[0].IsValid())
		return SVector2f::Zero();
	return { myTextures[0]->GetWidth(), myTextures[0]->GetHeight() };
}
