#pragma once
template <typename T>
struct SVector3
{
public:
	constexpr SVector3() = default;

	constexpr SVector3(const T InX, const T InY, const T InZ)
		: X(InX), Y(InY), Z(InZ)
	{
	}

	T X, Y, Z;
};