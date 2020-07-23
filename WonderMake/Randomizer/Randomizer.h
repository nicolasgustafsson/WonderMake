#pragma once

#include "System/System.h"

#include <random>

class Randomizer
	: public System<>
{
public:
	Randomizer(Dependencies&& aDependencies);

	template <typename T = f32>
	T GetRandomNumber(T aMin = 0, T aMax = 1);

	bool GetRandomBool();

	template <typename T, T... TOptions>
	T SelectOne()
	{
		const size_t numberOfOptions = sizeof...(TOptions);

		const size_t selectedOption = GetRandomNumber<size_t>(0, numberOfOptions - 1);

		std::array<T, numberOfOptions> optionsAsArray = { { TOptions... } };

		return optionsAsArray[selectedOption];
	}


private:
	std::random_device myRandomDevice;
	std::mt19937 myRandomGenerator;
};

REGISTER_SYSTEM(Randomizer);

template <typename T /*= f32*/>
T Randomizer::GetRandomNumber(T aMin, T aMax)
{
	if constexpr (std::is_integral_v<T>)
	{
		std::uniform_int_distribution<T> distribution(aMin, aMax);
		return distribution(myRandomGenerator);
	}
	else
	{
		std::uniform_real_distribution<T> distribution(aMin, aMax);
		return distribution(myRandomGenerator);
	}
}
