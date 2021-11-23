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
	SVector2f End { 1.0f, 1.0f };
	SVector2f ControlFirst { WmMath::Lerp(Start, End, 0.3334f) };
	SVector2f ControlSecond{ WmMath::Lerp(Start, End, 0.6667f) };

	//SVector2f Third{ 2.0f, 0.0f };
	//SVector2f ControlThird{ WmMath::Lerp(End, Third, 0.6667f) };

	EAlterStatus Inspect();

	bool ShouldCenterEditor = false;

private:
	SVector2f* SelectedPoint = nullptr;

};

