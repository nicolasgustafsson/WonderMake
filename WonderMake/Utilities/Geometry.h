#pragma once

struct SRectangle
{
	f32 Left{};
	f32 Top{};
	f32 Right{};
	f32 Bottom{};

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
};