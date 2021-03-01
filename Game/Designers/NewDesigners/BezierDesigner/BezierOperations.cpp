#include "pch.h"
#include "BezierOperations.h"
#include "../Sketch/Sketch.h"
#include "BezierAttributes.h"
#include "Randomizer/Randomizer.h"

bool DesignBezier::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SStartPositionAttribute>() 
		&& aSketch.ContainsAttribute<SEndPositionAttribute>()
		&& aSketch.ContainsAttribute<SControlPointsAttribute>();
}

void DesignBezier::Perform(Sketch& aSketch) const
{
	const SVector2f start = aSketch.GetAttribute<SStartPositionAttribute>()->Position;
	const SVector2f end = aSketch.GetAttribute<SEndPositionAttribute>()->Position;
	SVector2f firstControl = aSketch.GetAttribute<SControlPointsAttribute>()->FirstControl;
	SVector2f secondControl = aSketch.GetAttribute<SControlPointsAttribute>()->SecondControl;

	BezierCurve curve
	{
		start,
		end,
		firstControl,
		secondControl
	};

	SDesignedObjectAttribute<BezierCurve> designedObject;
	designedObject.FinishedDesign = curve;
	aSketch.AddAttribute(designedObject);
}

bool AddMissingBezierAttributes::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void AddMissingBezierAttributes::Perform(Sketch& aSketch) const
{
	if (!aSketch.ContainsAttribute<SStartPositionAttribute>())
	{
		SystemPtr<Randomizer> rand;
		SStartPositionAttribute start;
		start.Position = SVector2f(rand->GetRandomNumber<f32>(), rand->GetRandomNumber<f32>()).GetNormalized() * 100.f;

		aSketch.AddAttribute(std::move(start));
	}

	if (!aSketch.ContainsAttribute<SEndPositionAttribute>())
	{
		SEndPositionAttribute end;
		SystemPtr<Randomizer> rand;
		end.Position = SVector2f(rand->GetRandomNumber<f32>(), rand->GetRandomNumber<f32>()).GetNormalized() * 100.f;

		aSketch.AddAttribute<SEndPositionAttribute>(std::move(end));
	}
}

bool CreateControlPoints::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SStartPositionAttribute>() && aSketch.ContainsAttribute<SEndPositionAttribute>();
}

void CreateControlPoints::Perform(Sketch& aSketch) const
{
	SystemPtr<Randomizer> rand;

	const SVector2f start = aSketch.GetAttribute<SStartPositionAttribute>()->Position;
	const SVector2f end = aSketch.GetAttribute<SEndPositionAttribute>()->Position;
	SVector2f firstControl;
	SVector2f secondControl;

	const SVector2f straightLine = end - start;
	const SVector2f normal = (SVector2f(-straightLine.Y, straightLine.X) * -1.f).GetNormalized();

	const bool isCurved = true;
	if (auto shape = aSketch.GetAttribute<SCurvedShapeAttribute>())
	{
		const f32 minAllowedDistance = shape->MinimumDistance;
		const f32 maxAllowedDistance = shape->MaximumDistance;

		firstControl.X = rand->GetRandomNumber<f32>(start.X + normal.X * minAllowedDistance, start.X + normal.X * maxAllowedDistance);
		firstControl.Y = rand->GetRandomNumber<f32>(start.Y + normal.Y * minAllowedDistance, start.Y + normal.Y * maxAllowedDistance);
		secondControl.X = rand->GetRandomNumber<f32>(end.X + normal.X * minAllowedDistance, end.X + normal.X * maxAllowedDistance);
		secondControl.Y = rand->GetRandomNumber<f32>(end.Y + normal.Y * minAllowedDistance, end.Y + normal.Y * maxAllowedDistance);
	}
	else
	{
		//assume linearity
		firstControl = (end - start) / 2.f + start;
		secondControl = (end - start) / 2.f + start;
	}
	SControlPointsAttribute attribute;
	attribute.FirstControl = firstControl;
	attribute.SecondControl = secondControl;

	aSketch.AddAttribute(std::move(attribute));
}
