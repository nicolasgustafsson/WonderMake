#pragma once

#include "Utilities/Vector.h"
#include "Utilities/Geometry/Geometry.h"
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
        [[nodiscard]] inline SCircle GetCircle() const noexcept;
		f32							Radius = 0.f;
	};

    SCircle SSphere::GetCircle() const noexcept
    {
        return {Position, Radius};
    }

    struct SCollisionLine
		: public SBase
	{
		SCollisionLine() noexcept
			: SBase({ nullptr })
		{}

        [[nodiscard]] inline SVector2f GetLineEnd() const noexcept;
        [[nodiscard]] inline SSurface GetSurface() const noexcept;

		SVector2f					EndOffsetFromPosition = SVector2f::Zero();
		SDegreeF32					Rotation;
		f32							Width = 0.f;
	};

	SVector2f SCollisionLine::GetLineEnd() const noexcept
	{
		SVector2f temp = EndOffsetFromPosition;
		return Position + temp.RotateCounterClockwise(Rotation);
	}

    SSurface SCollisionLine::GetSurface() const noexcept
    {
        return {Position, GetLineEnd(), Width};
    }

    typedef std::variant<SSphere, SCollisionLine>	Shape;

	struct SCollisionInfo
	{
		const Shape& OriginalCollider;
		const Shape& OtherCollider;
	};
}
