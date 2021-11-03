#pragma once
class FloatSpline
{
public:
	SVector2f Start { 0.0f, 0.0f };
	SVector2f End { 100.0f, 100.0f };
	SVector2f ControlFirst { Start };
	SVector2f ControlSecond { End };

	void Inspect();

private:
	SVector2f* SelectedPoint = nullptr;
};

