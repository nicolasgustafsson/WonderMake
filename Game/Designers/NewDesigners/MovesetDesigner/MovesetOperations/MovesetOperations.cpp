#include "pch.h"
#include "MovesetOperations.h"
#include "../Moveset.h"
#include "../../Attribute/Attribute.h"
#include "../../Sketch/Sketch.h"

bool AddMove::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void AddMove::Perform(Sketch& aSketch) const
{
	SSwing swing;

	const f32 sidewayMultiplier = 1.0f;
	swing.SwingPath = BezierCurve(
		{ 50.f * sidewayMultiplier, -25.f },
		{ -50.f * sidewayMultiplier, -25.f },
		{ 50.f * sidewayMultiplier, 30.f },
		{ 0.f, 100.f });

	SGenericAttribute<SSwing> waieoteiorat;
	waieoteiorat.Attribute = swing;

	aSketch.AddAttribute(std::move(waieoteiorat));
}

bool SmashTogetherMoveset::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SGenericAttribute<SSwing>>();
}

void SmashTogetherMoveset::Perform(Sketch& aSketch) const
{
	SMoveset moveset;
	moveset.Swings.push_back(aSketch.GetAttribute<SGenericAttribute<SSwing>>()->Attribute);

	SDesignedObjectAttribute<SMoveset> attribute;
	attribute.FinishedDesign = moveset;

	aSketch.AddAttribute<SDesignedObjectAttribute<SMoveset>>(attribute);
}
