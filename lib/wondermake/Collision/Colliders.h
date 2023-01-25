#pragma once

#include "wondermake-utility/Vector.h"

#include <functional>
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
		_BaseFunctionality*			Functionality = nullptr;
		SVector2f					Position;

		size_t						Identifier;
		std::vector<SReaction>		Reactions;
	};

	struct SSphere
		: public SBase
	{
		f32							Radius = 0.f;
	};

	struct SAxisAlignedBoundingBox
		: public SBase
	{
		SVector2f					Dimensions;
	};

	struct SCollisionLine
		: public SBase
	{
		inline [[nodiscard]] SVector2f GetLineEnd() const noexcept;

		SVector2f					EndOffsetFromPosition = SVector2f::Zero();
		SDegreeF32					Rotation;
		f32							Width = 0.f;
	};

	SVector2f SCollisionLine::GetLineEnd() const noexcept
	{
		SVector2f temp = EndOffsetFromPosition;
		return Position + temp.RotateCounterClockwise(Rotation);
	}

	typedef std::variant<SSphere, SAxisAlignedBoundingBox, SCollisionLine>	Shape;

	struct SCollisionInfo
	{
		const Shape& OriginalCollider;
		const Shape& OtherCollider;
	};
}
