#include "Font.h"

#include "Resources/Resource.h"
#include "Utilities/Json/JsonExtensions.h"
#include "Utilities/Json/JsonUtility.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/SystemPtr.h"
#include "wondermake-base/WmLogTags.h"

#include <json/json.hpp>

WM_REGISTER_RESOURCE(Font);

Font::Font(const std::filesystem::path& aPath)
{
	std::optional<nlohmann::json> jsonFile = JsonUtility::LoadJson(aPath);

	if (jsonFile)
	{
		myFontInfo = jsonFile->get<SFontInfo>();

		myTextAtlas = SystemPtr<ResourceSystem<Texture>>()->GetResource(myFontInfo->AtlasPath);

		WmLogSuccess(TagWonderMake << "Created font with path " << aPath.string() << '.');
	}
}

SGlyphMetrics Font::GetGlyphMetrics(const u32 aUnicode) const
{
	if (!myFontInfo)
		return {};

	auto it = myFontInfo->GlyphMetrics.find(aUnicode);

	if (it == myFontInfo->GlyphMetrics.end())
		return {};
	

	return it->second;
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

	auto itKerning = myFontInfo->Kerning.find(aPreviousCharacter);

	if (itKerning == myFontInfo->Kerning.end())
		return 0.f;

	const auto& characterKerningMap = itKerning->second;

	auto characterIt = characterKerningMap.find(aNextCharacter);

	if (characterIt == characterKerningMap.end())
		return 0.f;

	return characterIt->second;
}
