#pragma once
#include "Utilities/Color.h"
namespace Palette
{
	constexpr static SColor PrimaryColor = SColor::Amaranth();
	constexpr static SColor SecondaryColor = SColor::SeaGreenCrayola();
	constexpr static SColor AccentColor = SColor::MiddleYellow();
	constexpr static SColor BlackBackground = SColor::EerieBlack();
	constexpr static SColor DarkBackground = SColor::Jet();
	constexpr static SColor MediumBackground = SColor::SpanishGray();
	constexpr static SColor LightBackground = SColor::Gainsboro();

	constexpr static SColor EnemyColor = PrimaryColor;
	constexpr static SColor PlayerColor = SecondaryColor;
};
