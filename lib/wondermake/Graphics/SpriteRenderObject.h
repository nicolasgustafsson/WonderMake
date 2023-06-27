#pragma once

#include "Graphics/RenderObject.h"

#include "wondermake-utility/SharedReference.h"

class TextureResource;

class SpriteRenderObject 
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Origin
		, EVertexAttribute::Scale
		, EVertexAttribute::Color
		, EVertexAttribute::Rotation>
{
public:
	SpriteRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram);
	
	inline void SetTexture(FileResourceRef<TextureResource> aResource)
	{
		myTextures.Clear();

		const auto textureSize = aResource->Size();

		myTextures.Add(std::move(aResource));

		myShaderSystem.SetProgramProperty(myShaderProgram, "TextureSize", SVector2f{ static_cast<f32>(textureSize.X), static_cast<f32>(textureSize.Y) });
	}

	inline [[nodiscard]] FileResourcePtr<TextureResource> GetTexture(size_t aIndex) const noexcept
	{
		if (aIndex >= myTextures.Count())
			return nullptr;

		return myTextures[aIndex];
	}

	void SetColor(const SColor aColor);

protected:
	virtual void RenderInternal() override
	{
		if (!myTextures.empty())
		{
			const auto textureSize = myTextures[0]->Size();

			myShaderSystem.SetProgramProperty(myShaderProgram, "TextureSize", SVector2f{ static_cast<f32>(textureSize.X), static_cast<f32>(textureSize.Y) });
		}

		using Parent = RenderObject<EVertexAttribute::Position, EVertexAttribute::Origin, EVertexAttribute::Scale, EVertexAttribute::Color, EVertexAttribute::Rotation>;

		Parent::RenderInternal();
	}

};
