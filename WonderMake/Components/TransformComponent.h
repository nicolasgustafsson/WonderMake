#pragma once
#include "Component.h"

#include "Serialization/Serialization.h"
#include "Utilities/Vector.h"

struct STransformComponent 
	: public SComponent
{
	SVector2f	Position;
	f32			Rotation = 0.f;
};

namespace meta
{
	template <>
	inline auto registerMembers<STransformComponent>()
	{
		return members(
			member("Position", &STransformComponent::Position),
			member("Rotation", &STransformComponent::Rotation)
		);
	}
}
