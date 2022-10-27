#pragma once

#include "wondermake-utility/Typedefs.h"

namespace Constants
{
#ifdef NDEBUG
	constexpr bool IsDebugging = false;
#else
	constexpr bool IsDebugging = true;
#endif

	constexpr bool EnableAssetHotReload = IsDebugging;

	constexpr f32 Pi = 3.141592f;
	constexpr f32 HalfPi = Pi / 2.f;
	constexpr f32 Tau = Pi * 2;
}