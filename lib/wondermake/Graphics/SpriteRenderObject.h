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
	
	inline void SetTexture(ResourceProxy<Texture> aResource)
	{
		myTextures.Clear();

		if (aResource.IsValid())
			myTextures.Add(aResource);

		if (!myTextures.empty() && myTextures[0].IsValid())
			myShaderProgram.SetProperty("TextureSize", SVector2f{ static_cast<f32>(myTextures[0]->GetWidth()), static_cast<f32>(myTextures[0]->GetHeight()) });
	}
	inline void SetTexture(const std::string_view aAssetLinkName)
	{
		SetTexture(SystemPtr<AssetDatabase<Texture>>()->GetResource(aAssetLinkName));
	}

	inline [[nodiscard]] ResourceProxy<Texture> GetTexture(size_t aIndex) const noexcept
	{
		if (aIndex >= myTextures.Count())
			return ResourceProxy<Texture>();

		return myTextures[aIndex];
	}

	void SetColor(const SColor aColor);
};

