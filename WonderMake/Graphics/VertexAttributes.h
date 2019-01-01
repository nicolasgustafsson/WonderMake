#pragma once
#include "../Utilities/Vector.h"

enum class EVertexAttribute
{
	Position,
	Color,
	Scale,
	Rotation,
	TexCoord
};

template<EVertexAttribute TVertexAttribute>
static auto constexpr GetValueFromAttribute()
{
	if constexpr (TVertexAttribute == EVertexAttribute::Color)
		return SColor();
	else if constexpr (TVertexAttribute == EVertexAttribute::Position)
		return SVector2f();
	else if constexpr (TVertexAttribute == EVertexAttribute::Scale)
		return SVector2f();
	else if constexpr (TVertexAttribute == EVertexAttribute::TexCoord)
		return SVector2f();
	else if constexpr (TVertexAttribute == EVertexAttribute::Rotation)
		return 0.f;
}