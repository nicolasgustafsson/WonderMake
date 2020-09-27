#include "pch.h"
#include "SpriteRenderObject.h"

SpriteRenderObject::SpriteRenderObject(const std::filesystem::path& aTexturePath)
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/SpriteVertex.vert"
		,	std::filesystem::current_path() / "Shaders/Geometry/SpriteGeometry.geom"
		,	std::filesystem::current_path() / "Shaders/Fragment/SpriteFragment.frag"
		,	aTexturePath
		,	1 })
{
	myShaderProgram.SetProperty("TextureSize", SVector2f{ static_cast<f32>(myTextures[0]->GetWidth()), static_cast<f32>(myTextures[0]->GetHeight())});
	SetAttribute<EVertexAttribute::Scale>(0, SVector2f{ 1.0f, 1.0f });
	SetAttribute<EVertexAttribute::Color>(0, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SpriteRenderObject::SetColor(const SColor aColor)
{
	SetAttribute<EVertexAttribute::Color>(0, aColor);
}
