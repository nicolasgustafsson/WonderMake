#pragma once

#include "Utilities/Vector.h"

#include <variant>

class _BaseFunctionality;
namespace Colliders
{
	struct SCollisionInfo;

	struct SReaction
	{
		template<typename TFunctionalityToReactAgainst>
		SReaction(std::function<void(TFunctionalityToReactAgainst&, SCollisionInfo)> aCallback) :
			Filter { typeid(TFunctionalityToReactAgainst).hash_code() }
		{
			Callback = [aCallback](_BaseFunctionality& aBaseFunctionality, const SCollisionInfo& aCollisionInfo) 
			{
				aCallback(static_cast<TFunctionalityToReactAgainst&>(aBaseFunctionality), aCollisionInfo);
			};
		}

		std::function<void(_BaseFunctionality&, const SCollisionInfo&)>	Callback;
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

		SVector2f					EndOffsetFromPosition = SVector2f::Zero();
		f32							Rotation = 0.f;
	};

	SVector2f SLine::GetLineEnd() const noexcept
	{
		SVector2f temp = EndOffsetFromPosition;
		return Position + temp.Rotate(Rotation);
	}

	typedef std::variant<SSphere, SLine>	Shape;

	struct SCollisionInfo
	{
		const Shape& OriginalCollider;
		const Shape& OtherCollider;
	};
}
