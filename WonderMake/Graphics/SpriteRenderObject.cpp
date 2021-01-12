#include "pch.h"
#include "SpriteRenderObject.h"

SpriteRenderObject::SpriteRenderObject(const std::string_view aTextureAssetLink)
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/Sprite.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/Sprite.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/Sprite.frag"
		,	aTextureAssetLink
		,	1 })
{
	if (myTextures[0].IsValid())
		myShaderProgram.SetProperty("TextureSize", SVector2f{ static_cast<f32>(myTextures[0]->GetWidth()), static_cast<f32>(myTextures[0]->GetHeight()) });

	SetAttribute<EVertexAttribute::Scale>(0, SVector2f{ 1.0f, 1.0f });
	SetAttribute<EVertexAttribute::Color>(0, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SpriteRenderObject::SetColor(const SColor aColor)
{
	SetAttribute<EVertexAttribute::Color>(0, aColor);
}
