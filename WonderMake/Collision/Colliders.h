#pragma once

#include "Utilities/Vector.h"

#include <variant>

class CollisionFunctionality;

namespace Colliders
{
	struct SBase
	{
		CollisionFunctionality*		CollisionFunctionality;
		SVector2f					Position;
	};

	struct SSphere
		: public SBase
	{
		SSphere() noexcept
			: SBase({ nullptr })
		{}

		f32							Radius = 0.f;
	};

	typedef std::variant<SSphere>	Shape;
}
