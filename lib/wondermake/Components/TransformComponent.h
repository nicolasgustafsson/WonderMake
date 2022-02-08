#pragma once
#include "Component.h"
#include "wondermake-utility/Vector.h"

template<typename TRepPosition, typename TRepRotation>
struct _STransformComponent2D
	: public SComponent
{
	using RepPosition = TRepPosition;
	using RepRotation = TRepRotation;

	RepPosition	Position = {};
	RepRotation	Rotation = {};
};

using STransformComponent2D = _STransformComponent2D<SVector2<f32>, SDegree<f32>>;
