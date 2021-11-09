#pragma once
#include"Utilities/WmMath.h"
#include "Utilities/Easing/EasingFunctions.h"

//quick and dirty class for making simple animations over values
template<typename T, f32 Time, decltype(WmEasings::Lerp<T>) TFunction = WmEasings::Lerp<T>> requires (WmMath::Interpolable<T> && Time > 0.f)
class AnimatedValue
{
public:
	template<typename U>
	AnimatedValue(U aInitialValue)
	{
		Current = From = To = aInitialValue;
	}

	T& operator=(T& aSetValue)
	{
		if (aSetValue == To)
			return Current;

		From = Current;
		To = aSetValue;

		SetTime = SystemPtr<TimeKeeper>()->GetGameTime();

		return Current;
	}

	operator T() const
	{
		UpdateCurrentValue();
		return Current;
	}

private:

	void UpdateCurrentValue() const
	{
		Current = TFunction(SRange<T>{From, To}, WmMath::Clamp(0.f, Time, SystemPtr<TimeKeeper>()->TimeSince(SetTime)) / Time);
	}

	mutable T Current{};
	T From{};
	T To{};

	f32 SetTime{ 0.f };
};

