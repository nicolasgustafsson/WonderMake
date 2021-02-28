#pragma once

#include "Imgui/ImguiInclude.h"
#include "Typedefs.h"

struct SColor
{
public:
	constexpr SColor() = default;

	constexpr SColor(const i32 aHexadecimalValue, f32 aAlpha = 1.0f)
		: SColor(static_cast<u8>((aHexadecimalValue >> 16) & 0xFF), static_cast<u8>((aHexadecimalValue >> 8) & 0xFF), static_cast<u8>((aHexadecimalValue) & 0xFF), (u8)(aAlpha * 255.f))
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

	constexpr bool operator==(const SColor& aOther) const { return aOther.R == R && aOther.G == G && aOther.B == B && aOther.A == A; }
	constexpr bool operator!=(const SColor& aOther) const { return !(aOther == *this); }

	[[nodiscard]] constexpr static SColor CornflowerBlue(const f32 aAlpha = 1.0f)	noexcept { return { 100.f / 255.f, 149.f / 255.f, 237.f / 255.f, aAlpha }; }
	[[nodiscard]] constexpr static SColor Grey(const f32 aAlpha = 1.0f)				noexcept { return { 0.85f, 0.85f, 0.85f, aAlpha }; }
	[[nodiscard]] constexpr static SColor White(const f32 aAlpha = 1.0f)			noexcept { return SColor(255_u8, 255_u8, 255_u8, (u8)(aAlpha * 255.f)); }
	[[nodiscard]] constexpr static SColor Black(const f32 aAlpha = 1.0f)			noexcept { return SColor(0_u8, 0_u8, 0_u8, (u8)(aAlpha * 255.f)); }
	[[nodiscard]] constexpr static SColor Yellow(const f32 aAlpha = 1.0f)			noexcept { return { 1.0f, 1.0f, 0.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColor Red(const f32 aAlpha = 1.0f)				noexcept { return { 1.0f, 0.0f, 0.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColor Green(const f32 aAlpha = 1.0f)			noexcept { return { 0.0f, 1.0f, 0.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColor Blue(const f32 aAlpha = 1.0f)				noexcept { return { 0.0f, 0.0f, 1.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColor ImperialRed(const f32 aAlpha = 1.0f)		noexcept { return SColor(0xE54B4B, aAlpha); }
	[[nodiscard]] constexpr static SColor BlueBell(const f32 aAlpha = 1.0f)			noexcept { return SColor(0x8789C0, aAlpha); }
	[[nodiscard]] constexpr static SColor KombuGreen(const f32 aAlpha = 1.0f)		noexcept { return SColor(0x334139, aAlpha); }
	[[nodiscard]] constexpr static SColor LightSalmon(const f32 aAlpha = 1.0f)		noexcept { return SColor(0xFFA987, aAlpha); }
	[[nodiscard]] constexpr static SColor Seashell(const f32 aAlpha = 1.0f)			noexcept { return SColor(0xF7EBE8, aAlpha); }
	[[nodiscard]] constexpr static SColor SpanishGray(const f32 aAlpha = 1.0f)		noexcept { return SColor(0x9E9694, aAlpha); }
	[[nodiscard]] constexpr static SColor Jet(const f32 aAlpha = 1.0f)				noexcept { return SColor(0x333333, aAlpha); }
	[[nodiscard]] constexpr static SColor RaisinBlack(const f32 aAlpha = 1.0f)		noexcept { return SColor(0x1E1E24, aAlpha); }
	[[nodiscard]] constexpr static SColor AmaranthPurple(const f32 aAlpha = 1.0f)	noexcept { return SColor(0xA23652, aAlpha); }
	[[nodiscard]] constexpr static SColor Zomp(const f32 aAlpha = 1.0f)				noexcept { return SColor(0x36A184, aAlpha); }
	[[nodiscard]] constexpr static SColor MinionYellow(const f32 aAlpha = 1.0f)		noexcept { return SColor(0xF1E254, aAlpha); }
	[[nodiscard]] constexpr static SColor DimGray(const f32 aAlpha = 1.0f)			noexcept { return SColor(0x6C6C6B, aAlpha); }
	[[nodiscard]] constexpr static SColor SilverChalice(const f32 aAlpha = 1.0f)	noexcept { return SColor(0xADADAD, aAlpha); }
	[[nodiscard]] constexpr static SColor Gainsboro(const f32 aAlpha = 1.0f)		noexcept { return SColor(0xE0E0E0, aAlpha); }
	[[nodiscard]] constexpr static SColor Amaranth(const f32 aAlpha = 1.0f)			noexcept { return SColor(0xE52B50, aAlpha); }
	[[nodiscard]] constexpr static SColor SeaGreenCrayola(f32 aAlpha = 1.0f)		noexcept { return SColor(0x2AE5BF, aAlpha); }
	[[nodiscard]] constexpr static SColor MiddleYellow(const f32 aAlpha = 1.0f)		noexcept { return SColor(0xFFE921, aAlpha); }
	[[nodiscard]] constexpr static SColor EerieBlack(const f32 aAlpha = 1.0f)		noexcept { return SColor(0x151514, aAlpha); }
};