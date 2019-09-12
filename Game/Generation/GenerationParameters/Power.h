#pragma once
#include "Generation/GeneratedObject.h"

struct SPower
{
public:
	SPower(const f32 aPower)
		: PowerValue(aPower)
	{
		
	}

	operator f32&() 
	{
		return PowerValue;
	}

	operator f32() const
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
	virtual void Weaken(const SPower aPower)
	{
		Strengthen(-aPower);
	};

	virtual void Rebalance() {};

	virtual SPower GetPower() const = 0;
};
