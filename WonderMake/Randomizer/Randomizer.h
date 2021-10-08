#pragma once

#include "System/System.h"

#include <random>

class Randomizer
	: public System<>
{
public:
	Randomizer();

	template <typename T = f32>
	T GetRandomNumber(T aMin = 0, T aMax = 1);

	bool GetRandomBool();

	template <SameType ... TOptions>
	auto SelectOne(TOptions... Options)
	{
		const size_t numberOfOptions = sizeof...(TOptions);

		const size_t selectedOption = GetRandomNumber<size_t>(0, numberOfOptions - 1);

		std::array<FirstType<TOptions...>, numberOfOptions> optionsAsArray = { { Options... } };

		return optionsAsArray[selectedOption];
	}

	SDegreeF32 GetRandomAngle()
	{
		return GetRandomNumber(0.f, 360.f);
	}

	//weighted towards middle right now; may change in future
	SVector2f GetRandomPositionInCircle(const SCircle aCircle) 
	{
		const SVector2f movement = SVector2f(0.f, 1.0f).RotateClockwise(GetRandomAngle()) * aCircle.Radius;

		return aCircle.Position + movement;
	}

private:
	std::random_device myRandomDevice;
	std::mt19937 myRandomGenerator;
};

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
