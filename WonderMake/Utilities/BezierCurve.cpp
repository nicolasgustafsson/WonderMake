#include "pch.h"
#include "BezierCurve.h"
#include "Math.h"

BezierCurve::BezierCurve(const SVector2f aStart, const SVector2f aEnd, const SVector2f aFirstControl, const SVector2f aSecondControl) noexcept
	: myStart(aStart), myEnd(aEnd), myFirstControl(aFirstControl), mySecondControl(aSecondControl)
{

}

//Shamelessly stolen from http://www.malinc.se/m/DeCasteljauAndBezier.php
SVector2f BezierCurve::GetLocationAt(const f32 aProgress) const noexcept
{
	return std::powf(1.0f - aProgress, 3.0f)* myStart + 3.f * std::powf(1.0f - aProgress, 2.0f) * aProgress * myFirstControl + 3.0f * (1.0f - aProgress)
		* std::powf(aProgress, 2.0f) * mySecondControl + std::powf(aProgress, 3.0f) * myEnd;
}

SVector2f BezierCurve::GetConstantLocationAt(f32 aProgress) const
{
	if (!myConstantLengthProgressList)
		EvaluatePoints();

	if (aProgress <= 0.0f || aProgress >= 1.0f)
		return GetLocationAt(aProgress);

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

void BezierCurve::Rotate(const SDegreeF32 aRotation)
{
	myStart.RotateCounterClockwise(aRotation);
	myEnd.RotateCounterClockwise(aRotation);
	myFirstControl.RotateCounterClockwise(aRotation);
	mySecondControl.RotateCounterClockwise(aRotation);

	myConstantLengthProgressList.reset();
}

void BezierCurve::Offset(const SVector2f aOffset)
{
	myStart += aOffset;
	myEnd += aOffset;
	myFirstControl += aOffset;
	mySecondControl += aOffset;

	myConstantLengthProgressList.reset();
}

float BezierCurve::GetLength() const noexcept
{
	return (myStart.DistanceTo(myEnd));
}

void BezierCurve::EvaluatePoints(const i32 aPointCount) const
{
	f32 length = 0.f;
	SVector2f previousLocation = myStart;

	myConstantLengthProgressList.emplace();
	myConstantLengthProgressList->reserve(aPointCount + 2);

	const SVector2f preLocation = GetLocationAt(-0.5f);

	myConstantLengthProgressList->push_back({ -0.5f, preLocation });

	for (i32 i = 0; i < aPointCount; i++)
	{
		const f32 progress = static_cast<f32>(i) / static_cast<f32>(aPointCount - 1);
		
		const SVector2f location = GetLocationAt(progress);

		length += (previousLocation - location).Length();

		previousLocation = location;

		myConstantLengthProgressList->push_back({ length, location });
	}


	const SVector2f postLocation = GetLocationAt(1.5f);

	myConstantLengthProgressList->push_back({ 1.5f, postLocation });

	for (i32 i = 0; i < aPointCount; i++)
	{
		(*myConstantLengthProgressList)[i].Progress /= length;
	}
}
