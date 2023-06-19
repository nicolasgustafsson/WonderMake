#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/Randomizer.h"

class RandomSystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STWonderMake>>
{
public:
	template <typename T = f32>
	[[nodiscard]] inline T GetRandomNumber(T aMin = 0, T aMax = 1)
	{
		return myRnd.GetRandomNumber<T>(aMin, aMax);
	}
	[[nodiscard]] inline bool GetRandomBool()
	{
		return myRnd.GetRandomBool();
	}
	template<typename TEnum>
	[[nodiscard]] inline TEnum SelectEnum()
	{
		return myRnd.SelectEnum<TEnum>();
	}

private:
	Randomizer myRnd;

};
