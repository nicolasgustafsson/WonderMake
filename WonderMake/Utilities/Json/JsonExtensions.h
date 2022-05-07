#pragma once
#include <json/json.hpp>
#include <filesystem>
#include <optional>
#include <sstream>
#include <chrono>
#include "Graphics/Text/FontMetrics.h"
#include "Utilities/Color.h"
#include "Utilities/Vector.h"


namespace std::filesystem
{
	inline void to_json(json& aJson, const path& aPath) {
		aJson = json{ {"path", aPath.wstring()} };
	}

	inline void from_json(const json& aJson, path& aPath) {
		wstring pathString;
		aJson.at("path").get_to(pathString);

		aPath = pathString;
	}
}

namespace std
{
	template<typename T>
	inline void to_json(json& aJson, const optional<T>& aOptional) 
	{
		if (aOptional)
			aJson = { {"hasValue", true}, {"value", *aOptional} };
		else
			aJson = { {"hasValue", false} };
	}

	template<typename T>
	inline void from_json(const json& aJson, optional<T>& aOptional)
	{
		if (!aJson["hasValue"].get<bool>())
			return;
		
		aOptional = aJson["value"].get<T>();
	}


	//inline void to_json(json& aJson, const std::chrono::time_point<std::chrono::system_clock>& aTimePoint)
	//{
	//	std::stringstream timepointstream;
	//	timepointstream << std::chrono::system_clock::now();
	//	//aJson = { {"X", aVector.X}, {"Y", aVector.Y} };
	//}
	//
	//inline void from_json(const json& aJson, std::chrono::time_point<std::chrono::system_clock>& aTimePoint)
	//{
	//
	//}

}

inline void to_json(json& aJson, const SColor& aColor)
{
	aJson = { {"R", aColor.R}, {"G", aColor.G}, {"B", aColor.B},{"A", aColor.A}, };
}

inline void to_json(json& aJson, const SVector2f& aVector)
{
	aJson = { {"X", aVector.X}, {"Y", aVector.Y} };
}

inline void from_json(const json& aJson, SVector2f& aVector)
{
	aVector.X = aJson["X"].get<f32>();
	aVector.Y = aJson["Y"].get<f32>();
}

inline void from_json(const json& aJson, SColor& aColor)
{
	aColor.R = aJson["R"].get<f32>();
	aColor.G = aJson["G"].get<f32>();
	aColor.B = aJson["B"].get<f32>();
	aColor.A = aJson["A"].get<f32>();
}

inline void from_json(const json& aJson, SFontAtlasMetrics& aMetrics)
{
	aMetrics.DistanceRange = aJson["distanceRange"].get<i32>();
	aMetrics.Size = aJson["size"].get<f32>();
	aMetrics.Width = aJson["width"].get<i32>();
	aMetrics.Height = aJson["height"].get<i32>();
}

inline void from_json(const json& aJson, SFontMetrics& aMetrics)
{
	aMetrics.EmSize = aJson["emSize"].get<i32>();
	aMetrics.LineHeight = aJson["lineHeight"].get<f32>();
	aMetrics.Ascender = aJson["ascender"].get<f32>();
	aMetrics.Descender = aJson["descender"].get<f32>();
	aMetrics.UnderlineY = aJson["underlineY"].get<f32>();
	aMetrics.UnderlineThickness = aJson["underlineThickness"].get<f32>();
}


inline void from_json(const json& aJson, SGlyphBounds& aMetrics)
{
	aMetrics.Left = aJson["left"].get<f32>();
	aMetrics.Bottom = aJson["bottom"].get<f32>();
	aMetrics.Right = aJson["right"].get<f32>();
	aMetrics.Top = aJson["top"].get<f32>();
}

inline void from_json(const json& aJson, SGlyphMetrics& aMetrics)
{
	aMetrics.Unicode = aJson["unicode"].get<u32>();
	aMetrics.Advance = aJson["advance"].get<f32>();
	aMetrics.PlaneBounds = aJson.value("planeBounds", SGlyphBounds{});
	aMetrics.AtlasBounds = aJson.value("atlasBounds", SGlyphBounds{});
}


inline void from_json(const json& aJson, SFontInfo& aFontInfo)
{
	aFontInfo.AtlasPath = aJson["image"].get<std::string>();
	aFontInfo.AtlasMetrics = aJson["atlas"].get<SFontAtlasMetrics>();
	aFontInfo.FontMetrics = aJson["metrics"].get<SFontMetrics>();

	for (auto& it : aJson["glyphs"].items())
	{
		SGlyphMetrics metrics = it.value().get<SGlyphMetrics>();

		//nicos: convert to 0-1 space
		metrics.AtlasBounds.Left /= aFontInfo.AtlasMetrics.Width;
		metrics.AtlasBounds.Right /= aFontInfo.AtlasMetrics.Width;
		metrics.AtlasBounds.Top /= aFontInfo.AtlasMetrics.Height;
		metrics.AtlasBounds.Bottom /= aFontInfo.AtlasMetrics.Height;

		aFontInfo.GlyphMetrics.Add(metrics.Unicode, metrics);
	}

	for (auto& it : aJson["kerning"].items())
	{
		u32 unicode1 = it.value()["unicode1"].get<u32>();
		u32 unicode2 = it.value()["unicode2"].get<u32>();
		f32 advance = it.value()["advance"].get<f32>();

		aFontInfo.Kerning[unicode1][unicode2] = advance;
	}
}
