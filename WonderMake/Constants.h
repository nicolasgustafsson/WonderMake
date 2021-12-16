#pragma once
namespace Constants
{
#ifdef CONFIGURATION_Release
	constexpr bool IsDebugging = false;
#else
	constexpr bool IsDebugging = true;
#endif

	constexpr bool ContainerBoundsChecking = IsDebugging;

	constexpr bool EnableAssetHotReload = true;//IsDebugging;

	constexpr f32 Pi = 3.141592f;
	constexpr f32 HalfPi = Pi / 2.f;
	constexpr f32 Tau = Pi * 2;
}

// Compiler definitions
// GCC  = __GNUC__
// MSVC = _MSC_VER
// clang = __clang__