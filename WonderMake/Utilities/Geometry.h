#pragma once
#include "TypeTraits/TypeTraits.h"

struct SRectangle
{
	f32 Left{};
	f32 Top{};
	f32 Right{};
	f32 Bottom{};

	template <SameType ... TPoints> requires std::is_same_v<FirstType<TPoints...>, SVector2f>
	static SRectangle GenerateBoundingBox(TPoints... aPoints)
	{
		const size_t numberOfOptions = sizeof...(TPoints);

		std::array<SVector2f, numberOfOptions> pointsAsArray = { { aPoints... } };

		SRectangle boundingBox;

		boundingBox.Left = pointsAsArray[0].X;
		boundingBox.Right = pointsAsArray[0].X;
		boundingBox.Top = pointsAsArray[0].Y;
		boundingBox.Bottom = pointsAsArray[0].Y;

		for (i32 i = 1; i < pointsAsArray.size(); i++)
		{
			SVector2f point = pointsAsArray[i];

			if (point.X < boundingBox.Left)
				boundingBox.Left = point.X;
			if (point.X > boundingBox.Right)
				boundingBox.Right = point.X;
			if (point.Y < boundingBox.Bottom)
				boundingBox.Bottom = point.Y;
			if (point.Y > boundingBox.Top)
				boundingBox.Top = point.Y;
		}

		return boundingBox;
	}

	template <typename TContainer>
	static SRectangle GenerateBoundingBox(TContainer aPointsContainer)
	{
		SRectangle boundingBox;

		boundingBox.Left = aPointsContainer[0].X;
		boundingBox.Right = aPointsContainer[0].X;
		boundingBox.Top = aPointsContainer[0].Y;
		boundingBox.Bottom = aPointsContainer[0].Y;

		for (auto&& point : aPointsContainer)
		{
			if (point.X < boundingBox.Left)
				boundingBox.Left = point.X;
			if (point.X > boundingBox.Right)
				boundingBox.Right = point.X;
			if (point.Y < boundingBox.Bottom)
				boundingBox.Bottom = point.Y;
			if (point.Y > boundingBox.Top)
				boundingBox.Top = point.Y;
		}

		return boundingBox;
	}

	f32 GetWidth() const
	{
		return Right - Left;
	}

	f32 GetHeight() const
	{
		return Top - Bottom;
	}

	SVector2f GetMiddle() const
	{
		return { Left + GetWidth() * 0.5f, Bottom + GetHeight() * 0.5f };
	}

	SVector2f GetTopLeft() const
	{
		return { Left, Top };
	}

	SVector2f GetTopRight() const
	{
		return { Right, Top };
	}

	SVector2f GetBottomLeft() const
	{
		return { Left, Bottom };
	}

	SVector2f GetBottomRight() const
	{
		return { Right, Bottom };
	}

	SRectangle GetSmallerRectangle(const f32 aSpacingAmount) const
	{
		return { Left + aSpacingAmount, Top - aSpacingAmount, Right - aSpacingAmount, Bottom + aSpacingAmount };
	}

	bool ContainsPoint(const SVector2f aPoint) const
	{
		return aPoint.X < Right&& aPoint.X > Left && aPoint.Y < Top&& aPoint.Y > Bottom;
	}

	void Move(const SVector2f aDelta)
	{
		Left += aDelta.X;
		Right += aDelta.X;
		Top += aDelta.Y;
		Bottom += aDelta.Y;
	}
	
	void ExpandMultiplicative(const f32 aMultiplier)
	{
		const auto center = GetMiddle();
		Left = center.X + (Left - center.X) * aMultiplier;
		Right = center.X + (Right - center.X) * aMultiplier;
		Top = center.Y + (Top - center.Y) * aMultiplier;
		Bottom = center.Y + (Bottom - center.Y) * aMultiplier;
	}
};

struct SCircle
{
	SVector2f Position;
	f32 Radius;

	SRectangle GetBoundingBox() const
	{
		return { Position.X - Radius, Position.Y + Radius, Position.X + Radius, Position.Y - Radius };
	}

	bool ContainsPoint(const SVector2f aPoint) const
	{
		return Position.DistanceTo(aPoint) < Radius;
	}
};