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

	struct SLine
		: public SBase
	{
		SLine() noexcept
			: SBase({ nullptr })
		{}

		inline [[nodiscard]] SVector2f GetLineEnd() const noexcept;

		SVector2f					EndOffsetFromPosition = {0.f, 0.f};
		f32							Rotation = 0.f;
	};

	SVector2f SLine::GetLineEnd() const noexcept
	{
		SVector2f temp = EndOffsetFromPosition;
		return Position + temp.Rotate(Rotation);
	}

	typedef std::variant<SSphere, SLine>	Shape;
}
