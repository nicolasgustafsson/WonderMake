#pragma once

#include "Imgui/ImguiInclude.h"
#include "Typedefs.h"

struct SColor
{
public:
	constexpr SColor() = default;

	constexpr SColor(const i32 aHexadecimalValue)
		: SColor(static_cast<u8>((aHexadecimalValue >> 16) & 0xFF), static_cast<u8>((aHexadecimalValue >> 8) & 0xFF), static_cast<u8>((aHexadecimalValue) & 0xFF))
	{

	}

	constexpr SColor(const u8 aRed, const u8 aGreen, const u8 aBlue, const u8 aAlpha = 255)
		: SColor(static_cast<f32>(aRed) / 255.f, static_cast<f32>(aGreen) / 255.f, static_cast<f32>(aBlue) / 255.f, static_cast<f32>(aAlpha) / 255.f)
	{

	}

	constexpr SColor(const f32 aRed, const f32 aGreen, const f32 aBlue, const f32 aAlpha = 1.0f) 
		: R(aRed), G(aGreen), B(aBlue), A(aAlpha)
	{
	
	}

	f32 R = 1.0f;
	f32 G = 1.0f;
	f32 B = 1.0f;
	f32 A = 1.0f;

	operator ImColor() { return ImColor(R, G, B, A); }
	operator ImVec4() { return ImVec4(R, G, B, A); }

	const static SColor CornflowerBlue;
	const static SColor Grey;
	const static SColor White;
	const static SColor Yellow;
	const static SColor Red;
	const static SColor Green;
	const static SColor Blue;

	const static SColor ImperialRed;
	const static SColor BlueBell;
	const static SColor KombuGreen;
	const static SColor LightSalmon;
	const static SColor Seashell; 
	const static SColor SpanishGray;
	const static SColor Jet;
	const static SColor RaisinBlack;
	const static SColor AmaranthPurple;
	const static SColor Zomp;
	const static SColor MinionYellow;
	const static SColor DimGray;
	const static SColor SilverChalice;
	const static SColor Gainsboro;
	const static SColor Amaranth;
	const static SColor SeaGreenCrayola;
	const static SColor MiddleYellow;
	const static SColor EerieBlack;

};

