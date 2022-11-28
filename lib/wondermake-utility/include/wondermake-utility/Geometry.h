#pragma once

#include "wondermake-utility/Math.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/Vector.h"

template<WmMath::Arithmetic TRep>
struct SRectangle
{
	using Rep = TRep;

	TRep Left{};
	TRep Top{};
	TRep Right{};
	TRep Bottom{};

	inline constexpr [[nodiscard]] TRep GetWidth() const noexcept
	{
		return Right - Left;
	}
	inline constexpr [[nodiscard]] TRep GetHeight() const noexcept
	{
		return Top - Bottom;
	}
	inline constexpr [[nodiscard]] SVector2<TRep> GetDimensions() const noexcept
	{
		return { GetWidth(), GetHeight() };
	}

	inline constexpr [[nodiscard]] SVector2<TRep> GetMiddle() const noexcept
	{
		return { Left + GetWidth() / 2, Bottom + GetHeight() / 2 };
	}
	inline constexpr [[nodiscard]] SVector2<TRep> GetTopLeft() const noexcept
	{
		return { Left, Top };
	}
	inline constexpr [[nodiscard]] SVector2<TRep> GetTopRight() const noexcept
	{
		return { Right, Top };
	}
	inline constexpr [[nodiscard]] SVector2<TRep> GetBottomLeft() const noexcept
	{
		return { Left, Bottom };
	}
	inline constexpr [[nodiscard]] SVector2<TRep> GetBottomRight() const noexcept
	{
		return { Right, Bottom };
	}

	inline constexpr [[nodiscard]] SRectangle GetSmallerRectangle(const TRep aSpacingAmount) const noexcept
	{
		return { Left + aSpacingAmount, Top - aSpacingAmount, Right - aSpacingAmount, Bottom + aSpacingAmount };
	}

	inline constexpr [[nodiscard]] bool ContainsPoint(const SVector2<TRep> aPoint) const noexcept
	{
		return aPoint.X < Right
			&& aPoint.X > Left
			&& aPoint.Y < Top
			&& aPoint.Y > Bottom;
	}

	inline constexpr [[nodiscard]] void Move(const SVector2<TRep>& aDelta) noexcept
	{
		Left	+= aDelta.X;
		Right	+= aDelta.X;
		Top		+= aDelta.Y;
		Bottom	+= aDelta.Y;
	}
	inline constexpr [[nodiscard]] void ExpandMultiplicative(const TRep& aMultiplier) noexcept
	{
		const auto center = GetMiddle();

		Left	= center.X + (Left		- center.X)	* aMultiplier;
		Right	= center.X + (Right		- center.X)	* aMultiplier;
		Top		= center.Y + (Top		- center.Y)	* aMultiplier;
		Bottom	= center.Y + (Bottom	- center.Y)	* aMultiplier;
	}
};

template<WmMath::Arithmetic TRep>
struct SCircle
{
	using Rep = TRep;

	SVector2<TRep>	Position;
	TRep			Radius;

	inline constexpr [[nodiscard]] SRectangle<TRep> GetBoundingBox() const noexcept
	{
		return { Position.X - Radius, Position.Y + Radius, Position.X + Radius, Position.Y - Radius };
	}

	inline constexpr [[nodiscard]] bool ContainsPoint(const SVector2f& aPoint) const noexcept
	{
		return Position.DistanceTo(aPoint) < Radius;
	}
	
	inline constexpr [[nodiscard]] void Move(const SVector2<TRep>& aDelta) noexcept
	{
		Position += aDelta;
	}
};

using SRectangleF = SRectangle<f32>;
