#pragma once
#include "Graphics/Texture.h"
#include "Resources/Resource.h"
#include "FontMetrics.h"

class Font
	: public Resource
	, private NonCopyable
{
public:
	Font(const std::filesystem::path& aPath);
	~Font();

	SGlyphMetrics GetGlyphMetrics(const u32 aUnicode) const;
	SFontMetrics GetFontMetrics() const;
	SFontAtlasMetrics GetFontAtlasMetrics() const;

	ResourceProxy<Texture> GetTexture() const;
	f32 GetKerning(const u32 aPreviousCharacter, const u32 aNextCharacter) const;
private:
	ResourceProxy<Texture> myTextAtlas;

	std::optional<SFontInfo> myFontInfo;

	//	void Bind(const u32 aTextureSlot = 0);
	//
	//	i32 GetWidth() const noexcept { return myWidth; }
	//	i32 GetHeight() const noexcept { return myHeight; }
	//
	//private:
	//	std::filesystem::path myPath;
	//	u32 myTextureHandle;
	//
	//	i32 myWidth;
	//	i32 myHeight;
};
