#pragma once

#include "wondermake-utility/Typedefs.h"

#include <limits>

enum class ETestEnumI32 : i32
{
	Zero = 0,
	One = 1,
	Min = std::numeric_limits<i32>::min(),
	Max = std::numeric_limits<i32>::max()
};
enum class ETestEnumU32 : u32
{
	Zero = 0,
	One = 1,
	Max = std::numeric_limits<i32>::max()
};
