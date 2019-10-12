#include "pch.h"
#include "BezierCurve.h"

BezierCurve::BezierCurve(const SVector2f aStart, const SVector2f aEnd, const SVector2f aFirstControl, const SVector2f aSecondControl)
	: myStart(aStart), myEnd(aEnd), myFirstControl(aFirstControl), mySecondControl(aSecondControl)
{

}

//Shamelessly stolen from http://www.malinc.se/m/DeCasteljauAndBezier.php
SVector2f BezierCurve::GetLocationAt(const f32 aProgress) const noexcept
{
	return std::powf(1.0f - aProgress, 3.0f)* myStart + 3.f * std::powf(1.0f - aProgress, 2.0f) * aProgress * myFirstControl + 3.0f * (1.0f - aProgress)
		* std::powf(aProgress, 2.0f) * mySecondControl + std::powf(aProgress, 3.0f) * myEnd;
}

SVector2f BezierCurve::GetConstantLocationAt(const f32 aProgress)
{
	if (!myConstantLengthProgressList)
		EvaluatePoints();

	auto firstLocation = std::lower_bound(myConstantLengthProgressList->begin(), myConstantLengthProgressList->end(), aProgress,
		[](SCachedBezierConstantLocation& first, const f32 second )
		{
			return first.Progress < second;
		});

	if (firstLocation == myConstantLengthProgressList->end())
		return ((*firstLocation).Location);

	auto secondLocation = firstLocation;
	firstLocation--;
	
	const SVector2f locationDelta = (*secondLocation).Location - (*firstLocation).Location;
	const f32 progressDelta = (*secondLocation).Progress - (*firstLocation).Progress;

	const f32 progressBetweenLocations = (aProgress - (*firstLocation).Progress) / progressDelta;
	
	return ((*firstLocation).Location) + progressBetweenLocations * locationDelta;
}

void BezierCurve::EvaluatePoints(const i32 aPointCount)
{
	f32 length = 0.f;
	SVector2f previousLocation = myStart;

	myConstantLengthProgressList.emplace();
	myConstantLengthProgressList->reserve(aPointCount);

	for (i32 i = 0; i < aPointCount; i++)
	{
		const f32 progress = static_cast<f32>(i) / static_cast<f32>(aPointCount - 1);
		
		const SVector2f location = GetLocationAt(progress);

		length += (previousLocation - location).Length();

		previousLocation = location;

		myConstantLengthProgressList->push_back({ length, location });
	}

	for (i32 i = 0; i < aPointCount; i++)
	{
		(*myConstantLengthProgressList)[i].Progress /= length;
	}
}
