#pragma once
#include "../../../Attribute/Attribute.h"
#include "Typedefs.h"
#include "Utilities/BezierCurve.h"

enum class ESwingType
{
	Stab,
	Slash
};

//Boilerplate = creating new classes for every attribute - can we make some template instead or something?

struct SSwingTypeAttribute : public SAttribute
{
	ESwingType Type;
};

struct SSwingPathAttribute : public SAttribute
{
	BezierCurve Path;
};

struct SStepLengthAttribute : public SAttribute
{
	f32 Length;
};

struct SSwingChargeTimeAttribute : public SAttribute
{
	f32 Time;
};

struct SSwingTimeAttribute : public SAttribute
{
	f32 Time;
};
