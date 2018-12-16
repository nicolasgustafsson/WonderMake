#pragma once

#include "Typedefs.h"

constexpr u32 ThreadCount = 3;

enum class EThreadId
{
	Logic = 0,
	Render,
	File
};
