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
};