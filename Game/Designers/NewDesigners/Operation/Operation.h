#pragma once

class Sketch;

class Operation
{
public:
	virtual ~Operation() = default;
	virtual bool IsEligible(const Sketch& aSketch) const = 0;

	virtual void Perform(Sketch& aSketch) const = 0;
};