#pragma once
template <typename T>
struct SVector4
{
public:
	constexpr SVector4() = default;

	constexpr SVector4(const T InX, const T InY, const T InZ, const T InW)
		: X(InX), Y(InY), Z(InZ), W(InW)
	{
	}

	T X, Y, Z, W;
};