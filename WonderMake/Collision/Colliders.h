#pragma once

#include "Utilities/Vector.h"

#include <variant>

class _BaseFunctionality;

namespace Colliders
{
	struct SReaction
	{
		template<typename TFunctionalityToReactTo>
		SReaction(std::function<void(TFunctionalityToReactTo&)> aCallback) :
			Filter { typeid(TFunctionalityToReactTo).hash_code() }
		{
			Callback = [aCallback](_BaseFunctionality& aBaseFunctionality) {aCallback(static_cast<TFunctionalityToReactTo&>(aBaseFunctionality));};
		}

		std::function<void(_BaseFunctionality&)>	Callback;
		size_t										Filter;
	};

	struct SBase
	{
		_BaseFunctionality*			Functionality;
		SVector2f					Position;

		size_t						Identifier;
		std::vector<SReaction>		Reactions;
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
