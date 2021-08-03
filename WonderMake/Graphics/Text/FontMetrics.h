#pragma once
struct SFontAtlasMetrics
{
	//Nicos: type and yOrigin are not covered here - if you want to use them, make sure to look in a json file to see how they are used and implement them
	i32 DistanceRange{};
	f32 Size{};
	i32 Width{};
	i32 Height{};
};

struct SFontMetrics
{
	i32 EmSize{};
	f32 LineHeight{};
	f32 Ascender{};
	f32 Descender{};
	f32 UnderlineY{};
	f32 UnderlineThickness{};
};

struct SGlyphBounds
{
	f32 Left{};
	f32 Bottom{};
	f32 Right{};
	f32 Top{};
};

struct SGlyphMetrics
{
	u32 Unicode{};
	f32 Advance{};

	SGlyphBounds PlaneBounds;
	SGlyphBounds AtlasBounds;
};

struct SFontInfo
{
	std::filesystem::path AtlasPath;
	SFontAtlasMetrics AtlasMetrics;
	SFontMetrics FontMetrics;

	Container<SGlyphMetrics, Iterable, Key<u32>> GlyphMetrics;

	Container<Container<f32, Key<u32>>, Key<u32>> Kerning;
};