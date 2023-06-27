#pragma once

#include "wondermake-ui/FontMetrics.h"

#include "wondermake-io/FileResource.h"
#include "wondermake-io/FileResourceProxy.h"

#include <optional>

class TextureResource;

class FontResource
	: public FileResource<>
{
public:
	inline FontResource(FileResourceRef<TextureResource> aAtlas, SFontInfo aInfo)
		: myAtlas(std::move(aAtlas))
		, myInfo(std::move(aInfo))
	{}

	[[nodiscard]] inline SGlyphMetrics GlyphMetrics(const u32 aUnicode) const noexcept
	{
		auto it = myInfo.GlyphMetrics.find(aUnicode);

		if (it == myInfo.GlyphMetrics.end())
			return {};

		return it->second;
	}
	[[nodiscard]] inline SFontMetrics FontMetrics() const noexcept
	{
		return myInfo.FontMetrics;
	}
	[[nodiscard]] inline SFontAtlasMetrics FontAtlasMetrics() const noexcept
	{
		return myInfo.AtlasMetrics;
	}
	[[nodiscard]] inline FileResourceRef<TextureResource> Atlas() const
	{
		return myAtlas;
	}

	[[nodiscard]] inline f32 Kerning(const u32 aPreviousCharacter, const u32 aNextCharacter) const noexcept
	{
		auto itKerning = myInfo.Kerning.find(aPreviousCharacter);

		if (itKerning == myInfo.Kerning.end())
			return 0.f;

		const auto& characterKerningMap = itKerning->second;

		auto characterIt = characterKerningMap.find(aNextCharacter);

		if (characterIt == characterKerningMap.end())
			return 0.f;

		return characterIt->second;
	}

private:
	FileResourceRef<TextureResource>	myAtlas;
	SFontInfo							myInfo;

};
