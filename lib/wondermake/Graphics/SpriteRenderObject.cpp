#include "SpriteRenderObject.h"

SpriteRenderObject::SpriteRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram)
	: RenderObject(SRenderObjectInfo
		{
			.ShaderSystem	= aShaderSystem,
			.ShaderProgram	= std::move(aShaderProgram),
			.TextureAsset	= nullptr,
			.VertexCount	= 1
		})
{
	if (!myTextures.empty() && myTextures[0]->IsValid())
		myShaderSystem.SetProgramProperty(myShaderProgram, "TextureSize", SVector2f{ static_cast<f32>(myTextures[0]->Size().X), static_cast<f32>(myTextures[0]->Size().Y) });

	SetAttribute<EVertexAttribute::Scale>(0, SVector2f{ 1.0f, 1.0f });
	SetAttribute<EVertexAttribute::Color>(0, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SpriteRenderObject::SetColor(const SColor aColor)
{
	SetAttribute<EVertexAttribute::Color>(0, aColor);
}
