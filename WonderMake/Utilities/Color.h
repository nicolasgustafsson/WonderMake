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

	constexpr bool operator==(const SColor& aOther) const { return aOther.R == R && aOther.G == G && aOther.B == B && aOther.A == A; }
	constexpr bool operator!=(const SColor& aOther) const { return !(aOther == *this); }

	[[nodiscard]] constexpr static SColor CornflowerBlue()	noexcept { return { 100.f / 255.f, 149.f / 255.f, 237.f / 255.f, 1.0f }; }
	[[nodiscard]] constexpr static SColor Grey()			noexcept { return { 0.85f, 0.85f, 0.85f, 1.0f };}
	[[nodiscard]] constexpr static SColor White()			noexcept { return SColor(255_u8, 255_u8, 255_u8); }
	[[nodiscard]] constexpr static SColor Black()			noexcept { return SColor(0_u8, 0_u8, 0_u8); }
	[[nodiscard]] constexpr static SColor Yellow()			noexcept { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
	[[nodiscard]] constexpr static SColor Red()				noexcept { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
	[[nodiscard]] constexpr static SColor Green()			noexcept { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
	[[nodiscard]] constexpr static SColor Blue()			noexcept { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
	[[nodiscard]] constexpr static SColor ImperialRed()		noexcept { return SColor(0xE54B4B); }
	[[nodiscard]] constexpr static SColor BlueBell()		noexcept { return SColor(0x8789C0);}
	[[nodiscard]] constexpr static SColor KombuGreen()		noexcept { return SColor(0x334139); }
	[[nodiscard]] constexpr static SColor LightSalmon()		noexcept { return SColor(0xFFA987); }
	[[nodiscard]] constexpr static SColor Seashell()		noexcept { return SColor(0xF7EBE8); }
	[[nodiscard]] constexpr static SColor SpanishGray()		noexcept { return SColor(0x9E9694); }
	[[nodiscard]] constexpr static SColor Jet()				noexcept { return SColor(0x333333); }
	[[nodiscard]] constexpr static SColor RaisinBlack()		noexcept { return SColor(0x1E1E24); }
	[[nodiscard]] constexpr static SColor AmaranthPurple()	noexcept { return SColor(0xA23652); }
	[[nodiscard]] constexpr static SColor Zomp()			noexcept { return SColor(0x36A184); }
	[[nodiscard]] constexpr static SColor MinionYellow()	noexcept { return SColor(0xF1E254); }
	[[nodiscard]] constexpr static SColor DimGray()			noexcept { return SColor(0x6C6C6B); }
	[[nodiscard]] constexpr static SColor SilverChalice()	noexcept { return SColor(0xADADAD); }
	[[nodiscard]] constexpr static SColor Gainsboro()		noexcept { return SColor(0xE0E0E0); }
	[[nodiscard]] constexpr static SColor Amaranth()		noexcept { return SColor(0xE52B50); }
	[[nodiscard]] constexpr static SColor SeaGreenCrayola() noexcept { return SColor(0x2AE5BF); }
	[[nodiscard]] constexpr static SColor MiddleYellow()	noexcept { return SColor(0xFFE921); }
	[[nodiscard]] constexpr static SColor EerieBlack()		noexcept { return SColor(0x151514); }
};