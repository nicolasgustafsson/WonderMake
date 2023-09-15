#pragma once

#include "wondermake-base/Component.h"

#include "wondermake-utility/Vector.h"

template<typename TRepPosition, typename TRepRotation>
struct _STransformComponent2D
	: public STraitedComponent<
		STrait::Set<
			STWonderMake>>
{
	using RepPosition = TRepPosition;
	using RepRotation = TRepRotation;

	[[nodiscard]] inline static std::string_view TypeName() noexcept
	{
		static auto typeName = "STransformComponent2D<" + static_cast<std::string>(Utility::GetTypeName<TRepPosition>()) + ", " + static_cast<std::string>(Utility::GetTypeName<TRepRotation>()) + '>';

		return typeName;
	}

	RepPosition	Position = {};
	RepRotation	Rotation = {};
};

using STransformComponent2D = _STransformComponent2D<SVector2<f32>, SDegree<f32>>;
