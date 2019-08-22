#pragma once
class BezierCurve
{
public:
	//Progress: 0-1
	SVector2f GetLocationAt(const f32 aProgress) const noexcept;

	SVector2f myStart, myEnd, myFirstControl, mySecondControl;
};

