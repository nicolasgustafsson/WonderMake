#pragma once
#include "Generation/GeneratedObject.h"

struct SPower
{
public:
	constexpr SPower(const f32 aPower)
		: PowerValue(aPower)
	{
		
	}

	constexpr operator f32&() 
	{
		return PowerValue;
	}

	constexpr operator f32() const
	{
		return PowerValue;
	}

private:
	f32 PowerValue;
};

template <>
class GenerationOperations<SPower>
{
public:
	virtual void Strengthen(const SPower aPower) = 0;

	void Weaken(const SPower aPower)
	{
		Strengthen(-aPower);
	};

	[[nodiscard]] virtual SPower GetPower() const = 0;
};
