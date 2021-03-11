#pragma once
#include "Utilities/Vector.h"
#include "Utilities/Color.h"

enum class EVertexAttribute
{
	Position,
	Color,
	Scale,
	Rotation,
	Origin,
	TexCoord,
	OneDimensionalUV,
	Depth,
	Progress
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
	else if constexpr (TVertexAttribute == EVertexAttribute::Origin)
		return SVector2f();
	else if constexpr (TVertexAttribute == EVertexAttribute::Rotation)
		return SRadianF32();
	else if constexpr (TVertexAttribute == EVertexAttribute::OneDimensionalUV)
		return 0.f;
	else if constexpr (TVertexAttribute == EVertexAttribute::Depth)
		return 0.f;
	else if constexpr (TVertexAttribute == EVertexAttribute::Progress)
		return 0.f;
}