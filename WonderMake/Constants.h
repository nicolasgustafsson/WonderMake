#pragma once
namespace Constants
{
#ifdef CONFIGURATION_Release
	constexpr bool IsDebugging = false;
#else
	constexpr bool IsDebugging = true;
#endif
}