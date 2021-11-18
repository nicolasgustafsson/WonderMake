#pragma once
#include "NodeGraph/SlotCustomization/SlotInputEdits.h"

class FloatSpline
{
public:
	bool operator==(const FloatSpline& aOther) const
	{
		return
			Start == aOther.Start &&
			End == aOther.End &&
			ControlFirst == aOther.ControlFirst &&
			ControlSecond == aOther.ControlSecond;
	}

	bool operator!=(const FloatSpline& aOther) const
	{
		return !(*this == aOther);
	}

	SVector2f Start { 0.0f, 0.0f };
	SVector2f End { 100.0f, 100.0f };
	SVector2f ControlFirst { Start };
	SVector2f ControlSecond { End };

	EAlterStatus Inspect();

private:
	SVector2f* SelectedPoint = nullptr;
};

