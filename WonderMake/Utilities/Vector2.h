#pragma once
template <typename T>
struct SVector2
{
public:
	constexpr SVector2() = default;

	constexpr SVector2(const T InX, const T InY)
		: X(InX), Y(InY)
	{
	}

	T X;
	T Y;
};