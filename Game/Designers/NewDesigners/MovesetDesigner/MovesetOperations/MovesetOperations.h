#pragma once
#include "../../Operation/Operation.h"

class DetermineAmountOfMoves : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};

class AddMoves : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};

class SmashTogetherMoveset : public Operation
{
public:
	virtual bool IsEligible(const Sketch& aSketch) const override;

	virtual void Perform(Sketch& aSketch) const override;
};