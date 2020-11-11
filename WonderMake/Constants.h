#pragma once
namespace Constants
{
#ifdef CONFIGURATION_Release
	constexpr bool IsDebugging = true;
#else
	constexpr bool IsDebugging = true;
#endif

	constexpr bool EnableAssetHotReload = IsDebugging;

	constexpr f32 Pi = 3.141592f;
	constexpr f32 HalfPi = Pi / 2.f;
	constexpr f32 Tau = Pi * 2;
}