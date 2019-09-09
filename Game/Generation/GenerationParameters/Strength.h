#pragma once
#include "Generation/GeneratedObject.h"

struct SStrength
{
public:
	explicit SStrength(const f32 aStrength)
		: StrengthValue(aStrength)
	{
		
	}

private:
	f32 StrengthValue;
};

template <>
class GenerationOperations<SStrength>
{
public:
	virtual void Strengthen(const SStrength aStrength) = 0;
	virtual void Weaken(const SStrength aStrength) = 0;

	virtual void Rebalance() {};

	virtual SStrength GetStrength() const = 0;
};
