#pragma once

#include "Utilities/Vector.h"

class CollisionFunctionality;

namespace Colliders
{
	enum class Type
		: u8
	{
		Sphere
	};

	struct SBase
	{
		CollisionFunctionality* CollisionFunctionality;
		Type					Type;
		SVector2f				Position;
	};

	struct SSphere
		: public SBase
	{
		SSphere() noexcept
			: SBase({ nullptr, Type::Sphere })
		{}

		f32						Radius = 0.f;
	};
}
