#pragma once
#include "../../../Operation/Operation.h"

class FinalizeSwing : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};

class GenerateSwingPath : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};

