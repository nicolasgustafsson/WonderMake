#pragma once
#include "../Operation/Operation.h"

class Sketch;

class AddMissingBezierAttributes : public Operation
{
public:
	bool IsEligible(const Sketch& aSketch) const override;
	void Perform(Sketch& aSketch) const override;
};

class CreateControlPoints : public Operation
{
public:
	bool IsEligible(const Sketch& aSketch) const override;
	void Perform(Sketch& aSketch) const override;
};

class DesignBezier : public Operation
{
	
public:
	bool IsEligible(const Sketch& aSketch) const override;
	void Perform(Sketch& aSketch) const override;
};