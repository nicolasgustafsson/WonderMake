#include "pch.h"
#include "SwingOperations.h"
#include "Weapons/WeaponSwing.h"
#include "../../../Attribute/Attribute.h"
#include "../../../Sketch/Sketch.h"
#include "../SwingAttributes/SwingAttributes.h"
#include "../../../BezierDesigner/BezierDesigner.h"
#include "../../../BezierDesigner/BezierAttributes.h"
#include "Randomizer/Randomizer.h"

bool FinalizeSwing::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SSwingInProgressAttribute>();
}

void FinalizeSwing::Perform(Sketch& aSketch) const
{
	const f32 sidewayMultiplier = 1.0f;

	//todo: these should pick from existing attributes
	SSwing swing;
	swing = aSketch.GetAttribute<SSwingInProgressAttribute>()->SwingInProgress;

	//boilerplate - we should be able to construct this inside the function call parameters
	SDesignedObjectAttribute<SSwing> designedSwingAttributes;
	designedSwingAttributes.FinishedDesign = swing;

	//todo - find out why we need to move; copying /should/ work here
	aSketch.AddAttribute(std::move(designedSwingAttributes));

	//todo - better error handling
}

bool GenerateSwingPath::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void GenerateSwingPath::Perform(Sketch& aSketch) const
{	
	Sketch bezierSketch;
	
	SVector2f start;
	SVector2f end;

	if (!aSketch.ContainsAttribute<SSwingInProgressAttribute>())
		aSketch.AddAttribute<SSwingInProgressAttribute>({});

	const f32 swingDirection = SystemPtr<Randomizer>()->SelectOne<f32, -1.f, 1.f>();

	if (aSketch.ContainsAttribute<SSwingTypeAttribute>())
	{
		switch (aSketch.GetAttribute<SSwingTypeAttribute>()->Type)
		{
		case ESwingType::Slash:
		{
			start = { 50.f * swingDirection, -25.f };
			end = { -50.f * swingDirection, -25.f };
			SCurvedShapeAttribute bezierShapeAttribute;
			bezierShapeAttribute.MinimumDistance = 50 * swingDirection;
			bezierShapeAttribute.MaximumDistance = 150 * swingDirection;
			bezierSketch.AddAttribute(std::move(bezierShapeAttribute));
			break;
		}

		case ESwingType::Stab:
			start = { 25 * swingDirection, -25 };
			end = { 0, 100 };
			break;
		}
	}

	SStartPositionAttribute startPositionAttribute;
	startPositionAttribute.Position = start;
	bezierSketch.AddAttribute(std::move(startPositionAttribute));

	SEndPositionAttribute endPositionAttribute;
	endPositionAttribute.Position = end;
	bezierSketch.AddAttribute(std::move(endPositionAttribute));
	
	auto swing = aSketch.GetAttribute<SSwingInProgressAttribute>();
	BezierDesigner bezierDesigner;
	swing->SwingInProgress.SwingPath = bezierDesigner.Design(bezierSketch);
}

bool DetermineSwingDamage::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void DetermineSwingDamage::Perform(Sketch& aSketch) const
{
	if (!aSketch.ContainsAttribute<SSwingInProgressAttribute>())
		aSketch.AddAttribute<SSwingInProgressAttribute>({});

	auto swing = aSketch.GetAttribute<SSwingInProgressAttribute>();

	if (aSketch.ContainsAttribute<SSwingThreatAttribute>())
	{
		auto swingThreat = aSketch.GetAttribute<SSwingThreatAttribute>();
		switch (swingThreat->SwingThreat)
		{
		case ESwingThreat::Pathetic:
			swing->SwingInProgress.DamageMultipiler = 0.25f;
			break;
		case ESwingThreat::Normal:
			swing->SwingInProgress.DamageMultipiler = 1.f;
			break;
		case ESwingThreat::Stronk:
			swing->SwingInProgress.DamageMultipiler = 3.f;
			break;
		}
	}
}

bool RandomizeSwingSpeed::IsEligible(const Sketch& aSketch) const
{
	return !aSketch.ContainsAttribute<SSwingSpeedAttribute>();
}

void RandomizeSwingSpeed::Perform(Sketch& aSketch) const
{
	SSwingSpeedAttribute swingSpeedAttribute;

	swingSpeedAttribute.SwingSpeed = SystemPtr<Randomizer>()->SelectOne<ESwingSpeed, ESwingSpeed::Snail, ESwingSpeed::Snake, ESwingSpeed::Cheetah>();
	aSketch.AddAttribute(std::move(swingSpeedAttribute));
}

bool DetermineSwingTimings::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void DetermineSwingTimings::Perform(Sketch& aSketch) const
{
	if (!aSketch.ContainsAttribute<SSwingInProgressAttribute>())
		aSketch.AddAttribute<SSwingInProgressAttribute>({});

	ESwingSpeed swingSpeed = aSketch.GetAttribute<SSwingSpeedAttribute>()->SwingSpeed;

	SSwing& currentSwing = aSketch.GetAttribute<SSwingInProgressAttribute>()->SwingInProgress;

	switch (swingSpeed)
	{
	case ESwingSpeed::Snail:
		currentSwing.ChargeTime *= 2.f;
		currentSwing.SwingTime *= 2.f;
		break;
	case ESwingSpeed::Snake:
		currentSwing.ChargeTime *= 1.f;
		currentSwing.SwingTime *= 1.f;
		break;
	case ESwingSpeed::Cheetah:
		currentSwing.ChargeTime *= 0.33f;
		currentSwing.SwingTime *= 0.33f;
		break;
	}
}
