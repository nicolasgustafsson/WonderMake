#pragma once

#include "Rotation.h"
#include "Imgui/ImguiInclude.h"
#include "Typedefs.h"

enum class EColorSpace
{
	LinearRGB,
	HSV
};


template <EColorSpace TColorSpace>
struct SColorTemplate
{
	
};

template <EColorSpace TFrom, EColorSpace TTo> //requires TFrom != TTo
SColorTemplate<TTo> ConvertColor(SColorTemplate<TFrom> aFrom) {
	return SColorTemplate<TTo>();
};

template <>
SColorTemplate<EColorSpace::HSV> ConvertColor(SColorTemplate<EColorSpace::LinearRGB> aFrom);

template <>
SColorTemplate<EColorSpace::LinearRGB> ConvertColor(SColorTemplate<EColorSpace::HSV> aFrom);

template <>
struct SColorTemplate<EColorSpace::LinearRGB>
{
public:
	constexpr SColorTemplate() = default;

	constexpr SColorTemplate(const i32 aHexadecimalValue, f32 aAlpha = 1.0f)
		: SColorTemplate(static_cast<u8>((aHexadecimalValue >> 16) & 0xFF), static_cast<u8>((aHexadecimalValue >> 8) & 0xFF), static_cast<u8>((aHexadecimalValue) & 0xFF), (u8)(aAlpha * 255.f))
	{

	}

	constexpr SColorTemplate(const u8 aRed, const u8 aGreen, const u8 aBlue, const u8 aAlpha = 255)
		: SColorTemplate(static_cast<f32>(aRed) / 255.f, static_cast<f32>(aGreen) / 255.f, static_cast<f32>(aBlue) / 255.f, static_cast<f32>(aAlpha) / 255.f)
	{

	}

	constexpr SColorTemplate(const f32 aRed, const f32 aGreen, const f32 aBlue, const f32 aAlpha = 1.0f)
		: R(aRed), G(aGreen), B(aBlue), A(aAlpha)
	{

	}

	f32 R = 1.0f;
	f32 G = 1.0f;
	f32 B = 1.0f;
	f32 A = 1.0f;

	operator SColorTemplate<EColorSpace::HSV>() const;

	operator ImColor() const { return ImColor(R, G, B, A); }
	operator ImVec4() const { return ImVec4(R, G, B, A); }

	constexpr bool operator==(const SColorTemplate& aOther) const { return aOther.R == R && aOther.G == G && aOther.B == B && aOther.A == A; }
	constexpr bool operator!=(const SColorTemplate& aOther) const { return !(aOther == *this); }

	[[nodiscard]] constexpr static SColorTemplate CornflowerBlue(const f32 aAlpha = 1.0f)		noexcept { return { 100.f / 255.f, 149.f / 255.f, 237.f / 255.f, aAlpha }; }
	[[nodiscard]] constexpr static SColorTemplate Grey(const f32 aAlpha = 1.0f)					noexcept { return { 0.85f, 0.85f, 0.85f, aAlpha }; }
	[[nodiscard]] constexpr static SColorTemplate White(const f32 aAlpha = 1.0f)				noexcept { return SColorTemplate(255_u8, 255_u8, 255_u8, (u8)(aAlpha * 255.f)); }
	[[nodiscard]] constexpr static SColorTemplate Black(const f32 aAlpha = 1.0f)				noexcept { return SColorTemplate(0_u8, 0_u8, 0_u8, (u8)(aAlpha * 255.f)); }
	[[nodiscard]] constexpr static SColorTemplate Yellow(const f32 aAlpha = 1.0f)				noexcept { return { 1.0f, 1.0f, 0.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColorTemplate Red(const f32 aAlpha = 1.0f)					noexcept { return { 1.0f, 0.0f, 0.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColorTemplate Green(const f32 aAlpha = 1.0f)				noexcept { return { 0.0f, 1.0f, 0.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColorTemplate Blue(const f32 aAlpha = 1.0f)					noexcept { return { 0.0f, 0.0f, 1.0f, aAlpha }; }
	[[nodiscard]] constexpr static SColorTemplate ImperialRed(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0xE54B4B, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate BlueBell(const f32 aAlpha = 1.0f)				noexcept { return SColorTemplate(0x8789C0, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate KombuGreen(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0x334139, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate LightSalmon(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0xFFA987, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate Seashell(const f32 aAlpha = 1.0f)				noexcept { return SColorTemplate(0xF7EBE8, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate SpanishGray(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0x9E9694, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate Jet(const f32 aAlpha = 1.0f)					noexcept { return SColorTemplate(0x333333, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate RaisinBlack(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0x1E1E24, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate AmaranthPurple(const f32 aAlpha = 1.0f)		noexcept { return SColorTemplate(0xA23652, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate Zomp(const f32 aAlpha = 1.0f)					noexcept { return SColorTemplate(0x36A184, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate MinionYellow(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0xF1E254, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate DimGray(const f32 aAlpha = 1.0f)				noexcept { return SColorTemplate(0x6C6C6B, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate SilverChalice(const f32 aAlpha = 1.0f)		noexcept { return SColorTemplate(0xADADAD, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate Gainsboro(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0xE0E0E0, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate Amaranth(const f32 aAlpha = 1.0f)				noexcept { return SColorTemplate(0xE52B50, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate SeaGreenCrayola(f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0x2AE5BF, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate MiddleYellow(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0xFFE921, aAlpha); }
	[[nodiscard]] constexpr static SColorTemplate EerieBlack(const f32 aAlpha = 1.0f)			noexcept { return SColorTemplate(0x151514, aAlpha); }
};

template <>
struct SColorTemplate<EColorSpace::HSV>
{
public:
	constexpr SColorTemplate() = default;

	constexpr SColorTemplate(const SDegree<f32> aHue, const f32 aSaturation, const f32 aValue, const f32 aAlpha = 1.0f)
		:Hue(aHue), Saturation(aSaturation), Value(aValue), Alpha(aAlpha)
	{

	}

	void Rotate(const SDegreeF32 aHueRotation)
	{
		Hue += aHueRotation;
	}

	operator SColorTemplate<EColorSpace::LinearRGB>() const;

	SDegreeF32 Hue = 0.f;
	f32 Saturation = 0.f;
	f32 Value = 1.f;
	f32 Alpha = 1.f;
};

using SColor = SColorTemplate<EColorSpace::LinearRGB>;
using SColorHSV = SColorTemplate<EColorSpace::HSV>;