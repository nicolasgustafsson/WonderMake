#pragma once
#include "NodeGraph/SlotCustomization/SlotInputEdits.h"

class FloatSpline
{
public:

	struct SSplinePoint
	{
		SVector2f Point;
		SVector2f Control;
		SVector2f ControlInverse;
	};

	bool operator==(const FloatSpline& aOther) const
	{
		aOther;
		return true;
			//First.Point == aOther.First.Point &&
			//Second.Point == aOther.Second.Point &&
			//First.Control == aOther.First.Control &&
			//Second.Control == aOther.Second.Control;
	}

	bool operator!=(const FloatSpline& aOther) const
	{
		return !(*this == aOther);
	}

	std::vector<SSplinePoint> Points{ {{{0.0f, 0.0f}, {0.33f, 0.33f}}, {{1.0f, 1.0f}, {0.66f, 0.66f}}, {{2.0f, 0.0f}, {1.5f, 0.66f}}, {{3.0f, 1.0f}, {2.5f, 0.33f}} } };


	f32 GetValue(const f32 aX);
	EAlterStatus Inspect();

	bool ShouldCenterEditor = false;

private:

	f32 GetNextInterestingScreenX(const f32 aX);
	std::pair<SSplinePoint&, SSplinePoint&> GetPointCombo(const f32 aX);
	SVector2f* SelectedPoint = nullptr;

};

