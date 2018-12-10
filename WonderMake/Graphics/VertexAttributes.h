#pragma once
#include "../Utilities/Vector.h"

enum class EVertexAttribute
{
	Position,
	Color,
	Scale,
	Rotation
};

template<EVertexAttribute VertexAttribute>
static auto constexpr GetValueFromAttribute()
{
	if constexpr (VertexAttribute == EVertexAttribute::Color)
		return SColor();
	else if constexpr (VertexAttribute == EVertexAttribute::Position)
		return SVector2f();
	else if constexpr (VertexAttribute == EVertexAttribute::Scale)
		return SVector2f();
	else if constexpr (VertexAttribute == EVertexAttribute::Rotation)
		return 0.f;
}