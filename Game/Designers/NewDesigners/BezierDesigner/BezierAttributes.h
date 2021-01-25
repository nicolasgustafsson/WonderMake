#pragma once
#include "../Attribute/Attribute.h"
#include "Typedefs.h"
#include <Utilities/Vector.h>

struct SCurvedShapeAttribute : public SAttribute
{
	f32 MinimumDistance;
	f32 MaximumDistance;
};

struct SStartPositionAttribute : public SAttribute
{
	SVector2f Position;
};

struct SEndPositionAttribute : public SAttribute
{
	SVector2f Position;
};

struct SControlPointsAttribute : public SAttribute
{
	SVector2f FirstControl;
	SVector2f SecondControl;
};

