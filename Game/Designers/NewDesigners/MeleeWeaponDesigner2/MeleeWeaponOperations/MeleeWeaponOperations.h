#pragma once
#include "../../Operation/Operation.h"

class SmashTogetherWeapon : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};


class GenerateMoveset : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};