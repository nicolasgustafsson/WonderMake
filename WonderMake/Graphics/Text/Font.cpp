#include "pch.h"
#include "Font.h"
#include "Utilities/Json/JsonUtility.h"

Font::Font(const std::filesystem::path& aPath)
{
	std::optional<nlohmann::json> jsonFile = JsonUtility::LoadJson(aPath);

	if (jsonFile)
	{
		myFontInfo = jsonFile->get<SFontInfo>();

		myTextAtlas = SystemPtr<ResourceSystem<Texture>>()->GetResource(myFontInfo->AtlasPath);

		WmLog(TagSuccess, "Created font with path ", aPath.string(), "!");
	}
}

Font::~Font()
{

}

SGlyphMetrics Font::GetGlyphMetrics(const u32 aUnicode) const
{
	if (!myFontInfo)
		return {};

	if (!myFontInfo->GlyphMetrics.KeyExists(aUnicode))
		return {};
	

	return myFontInfo->GlyphMetrics.Get(aUnicode);
}

SFontMetrics Font::GetFontMetrics() const
{
	return myFontInfo->FontMetrics;
}

SFontAtlasMetrics Font::GetFontAtlasMetrics() const
{
	if (!myFontInfo)
		return {};
	return myFontInfo->AtlasMetrics;
}

ResourceProxy<Texture> Font::GetTexture() const
{
	return myTextAtlas;
}

f32 Font::GetKerning(const u32 aPreviousCharacter, const u32 aNextCharacter) const
{
	if (!myFontInfo)
		return 0.f;

	if (!myFontInfo->Kerning.KeyExists(aPreviousCharacter))
		return 0.f;

	if (!myFontInfo->Kerning.Get(aPreviousCharacter).KeyExists(aNextCharacter))
		return 0.f;

	return myFontInfo->Kerning.Get(aPreviousCharacter).Get(aNextCharacter);
}
