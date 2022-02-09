#pragma once

#include "Typedefs.h"
#ifdef __GNUC__
#define COMPILER_GCC 1
#else
#define COMPILER_GCC 0
#endif

#ifdef _MSC_VER
#define COMPILER_MSVC 1
#else
#define COMPILER_MSVC 0
#endif


//CLANG IS NOT SUPPORTED AND WILL BREAK; I ONLY USE IT FOR CLANGD
#ifdef __clang__
#define COMPILER_clang 1
#else
#define COMPILER_clang 0
#endif

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

namespace Compiler
{
	constexpr bool Gcc = COMPILER_GCC;
	constexpr bool Msvc = COMPILER_MSVC;
	constexpr bool Clang = COMPILER_clang;
}

// Compiler definitions
// GCC  = __GNUC__
// MSVC = _MSC_VER
//clang = __clang__