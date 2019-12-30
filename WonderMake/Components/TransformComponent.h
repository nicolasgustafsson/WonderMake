#pragma once
#include "Component.h"
#include "Utilities/Vector.h"

struct STransformComponent 
	: public SComponent
{
	SVector2f	Position = SVector2f::Zero();
	f32			Rotation = 0.f;
};