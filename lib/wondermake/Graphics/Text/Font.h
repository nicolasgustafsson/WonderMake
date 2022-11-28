#pragma once
#include "Graphics/Text/FontMetrics.h"
#include "Graphics/Texture.h"
#include "Resources/Resource.h"
#include "Resources/ResourceProxy.h"

#include <filesystem>
#include <optional>

class Font
	: public Resource
	, private NonCopyable
{
public:
	Font(const std::filesystem::path& aPath);

	SGlyphMetrics GetGlyphMetrics(const u32 aUnicode) const;
	SFontMetrics GetFontMetrics() const;
	SFontAtlasMetrics GetFontAtlasMetrics() const;

	ResourceProxy<Texture> GetTexture() const;
	f32 GetKerning(const u32 aPreviousCharacter, const u32 aNextCharacter) const;

private:
	ResourceProxy<Texture> myTextAtlas;

	std::optional<SFontInfo> myFontInfo;

};
