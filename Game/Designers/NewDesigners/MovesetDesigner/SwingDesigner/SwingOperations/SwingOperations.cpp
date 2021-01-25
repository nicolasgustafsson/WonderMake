#include "pch.h"
#include "SwingOperations.h"
#include "Weapons/WeaponSwing.h"
#include "../../../Attribute/Attribute.h"
#include "../../../Sketch/Sketch.h"
#include "../SwingAttributes/SwingAttributes.h"
#include "../../../BezierDesigner/BezierDesigner.h"
#include "../../../BezierDesigner/BezierAttributes.h"

bool FinalizeSwing::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SSwingPathAttribute>();
}

void FinalizeSwing::Perform(Sketch& aSketch) const
{
	const f32 sidewayMultiplier = 1.0f;

	//todo: these should pick from existing attributes
	SSwing swing;
	swing.SwingPath = aSketch.GetAttribute<SSwingPathAttribute>()->Path;

	swing.ChargeTime;
	swing.StepLength;
	swing.SwingTime;

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

	const f32 sidewayMultiplier = 1.0f;
	if (aSketch.ContainsAttribute<SSwingTypeAttribute>())
	{
		switch (aSketch.GetAttribute<SSwingTypeAttribute>()->Type)
		{
		case ESwingType::Slash:
		{
			start = { 50.f, -25.f };
			end = { -50.f, -25.f };
			SCurvedShapeAttribute bezierShapeAttribute;
			bezierShapeAttribute.MinimumDistance = 50;
			bezierShapeAttribute.MaximumDistance = 150;
			bezierSketch.AddAttribute(std::move(bezierShapeAttribute));
			break;
		}

		case ESwingType::Stab:
			start = { 25, -25 };
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
	
	SSwingPathAttribute swingPath;
	BezierDesigner bezierDesigner;
	swingPath.Path = bezierDesigner.Design(bezierSketch);

	aSketch.AddAttribute(std::move(swingPath));
}
