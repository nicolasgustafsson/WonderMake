#pragma once
#include <random>

class Randomizer
	: public System
{
public:
	Randomizer();

	template <typename T = f32>
	T GetRandomNumber(T aMin = 0, T aMax = 1);

	bool GetRandomBool();

private:
	std::random_device myRandomDevice;
	std::mt19937 myRandomGenerator;
};

template <typename T /*= f32*/>
T Randomizer::GetRandomNumber(T aMin, T aMax)
{
	if constexpr (std::is_integral_v<T>)
	{
		std::uniform_int_distribution<T> distribution(aMin, aMax - 1);
		return distribution(myRandomGenerator);
	}
	else
	{
		std::uniform_real_distribution<T> distribution(aMin, aMax - 1);
		return distribution(myRandomGenerator);
	}
}

