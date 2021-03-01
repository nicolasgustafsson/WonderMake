#pragma once

#include "../Operation/Operation.h"

class RandomColorOperation : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};

class MovesetToColorOperation : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};
