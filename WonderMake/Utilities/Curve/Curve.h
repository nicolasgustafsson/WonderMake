#pragma once
#include "Utilities/Vector.h"
#include "Typedefs.h"

struct SCurvePoint
{
	SVector2f myPoint;
	SVector2f myControlPoint;
};

class Curve 
{

public:
	f32 GetValueAt(const float aProgress) const noexcept;

	f32 GetProgressAt(const float aPosition) const noexcept;

	f32 GetWidth() const noexcept;
	f32 GetHeight() const noexcept;

	f32 GetBottom() const noexcept;
	f32 GetTop() const noexcept;
	f32 GetRight() const noexcept;
	f32 GetLeft() const noexcept;


	std::vector<SVector2f> GetPoints() const noexcept;
	std::vector<SVector2f> GetPoints(const SVector4f aCanvas) const noexcept;

	SVector4f GetBoundingBox() const noexcept;
	SVector2f GetCenter() const noexcept;

	SCurvePoint myStart;
	SCurvePoint myEnd;

};

