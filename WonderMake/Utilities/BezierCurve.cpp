#include "pch.h"
#include "BezierCurve.h"

//Shamelessly stolen from http://www.malinc.se/m/DeCasteljauAndBezier.php
SVector2f BezierCurve::GetLocationAt(const f32 aProgress) const noexcept
{
	return std::powf(1.0f - aProgress, 3.0f)* myStart + 3.f * std::powf(1.0f - aProgress, 2.0f) * aProgress * myFirstControl + 3.0f * (1.0f - aProgress)
		* std::powf(aProgress, 2.0f) * mySecondControl + std::powf(aProgress, 3.0f) * myEnd;
}
