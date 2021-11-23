#pragma once

#include <optional>

struct SCachedBezierConstantLocation
{
	f32 Progress;
	SVector2f Location;
};

class BezierCurve
{
public:
	BezierCurve() = default;
	BezierCurve(const SVector2f aStart, const SVector2f aEnd, const SVector2f aFirstControl, const SVector2f aSecondControl) noexcept;

	bool operator==(const BezierCurve& aOther) const
	{
		return
			myStart == aOther.myStart &&
			myEnd == aOther.myEnd &&
			myFirstControl == aOther.myFirstControl &&
			mySecondControl == aOther.mySecondControl;
	}

	bool operator!=(const BezierCurve& aOther) const
	{
		return !(*this == aOther);
	}

	//Progress: 0-1
	SVector2f GetLocationAt(const f32 aProgress) const noexcept;
	SVector2f GetConstantLocationAt(f32 aProgress) const;

	void Rotate(const SDegreeF32 aRotation);
	void Offset(const SVector2f aOffset);

	[[nodiscard]] float GetLength() const noexcept;

	SVector2f myStart, myEnd, myFirstControl, mySecondControl;

private:
	void EvaluatePoints(const i32 aPointCount = 20) const;
	
	mutable std::optional<std::vector<SCachedBezierConstantLocation>> myConstantLengthProgressList;

};