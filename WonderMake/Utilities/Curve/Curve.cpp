#include "stdafx.h"
#include "Curve.h"
#include <algorithm>

f32 Curve::GetValueAt(const float aPosition) const noexcept
{
	const f32 progress = GetProgressAt(aPosition);

	const SVector2f value = std::pow((1 - progress), 3.f) * myStart.myPoint + 3.f * progress *
		std::pow((1 - progress), 2.f) * myStart.myControlPoint + 3.f * std::pow(progress, 2.0f) *
		(1.f - progress) * myEnd.myControlPoint + std::pow(progress, 3.0f) * myEnd.myPoint;

	//SVector2f real = MathUtility::Pow(1 - t
	//B(t) = (1 - t)^3 * P0 + 3 *(1-t)^2 * t * P1 + 3(1 - t)* t ^2 * P2 + t^3*P3
	// p = (1-t)^3 *P0 + 3*t*(1-t)^2*P1 + 3*t^2*(1-t)*P2 + t^3*P3 
	//const f32 value = (1.f - progress) * (1.f - progress) * myStart.Y + 2.f * (1.f - progress) * progress * myControlPoint.Y + progress * progress * myEnd.Y;

	return value.Y;
}

f32 Curve::GetProgressAt(const float aPosition) const noexcept
{
	const f32 width = GetWidth();

	if (width <= 0.f)
		return 1.0f;

	return aPosition / width;
}

f32 Curve::GetWidth() const noexcept
{
	const f32 width = GetRight() - GetLeft();

	return width;
}

f32 Curve::GetHeight() const noexcept
{

	const f32 height = GetTop() - GetBottom();

	return height;
}

f32 Curve::GetBottom() const noexcept
{
	return std::min(myEnd.myControlPoint.Y, std::min(myEnd.myPoint.Y, std::min(myStart.myPoint.Y, myStart.myControlPoint.Y)));
}

f32 Curve::GetTop() const noexcept
{
	return std::max(myEnd.myControlPoint.Y, std::max(myEnd.myPoint.Y, std::max(myStart.myPoint.Y, myStart.myControlPoint.Y)));
}

f32 Curve::GetRight() const noexcept
{
	return myEnd.myPoint.X;
}

f32 Curve::GetLeft() const noexcept
{
	return myStart.myPoint.X;
}

std::vector<SVector2f> Curve::GetPoints() const noexcept
{
	constexpr const i32 segments = 50;

	std::vector<SVector2f> points;

	for (i32 i = 0; i < segments; i++)
	{
		const f32 progress = static_cast<f32>(i) / static_cast<f32>(segments);
		const f32 x = GetWidth() * progress;

		const f32 y = GetValueAt(x);

		points.emplace_back(SVector2f(x + myStart.myPoint.X, y));
	}

	return points;
}

std::vector<SVector2f> Curve::GetPoints(const SVector4f aCanvas) const noexcept
{
	std::vector<SVector2f> points;

	constexpr const i32 segments = 50;

	for (i32 i = -1; i < segments + 1; i++)
	{
		const f32 progress = static_cast<f32>(i) / static_cast<f32>(segments);

		f32 x = aCanvas.X + (aCanvas.Z - aCanvas.X) * progress;

		const f32 y = GetValueAt(x);

		points.emplace_back(SVector2f(x + myStart.myPoint.X, y));
	}

	return points;
}

SVector4f Curve::GetBoundingBox() const noexcept
{
	const f32 right = GetRight();
	const f32 top = GetTop();
	const f32 left = GetLeft();
	const f32 bottom = GetBottom();

	return {left, bottom, right, top};
}

SVector2f Curve::GetCenter() const noexcept
{
	const SVector4f boundingBox = GetBoundingBox();

	const SVector2f position = { (boundingBox.X + boundingBox.Z) / 2.0f, (boundingBox.Y + boundingBox.W) / 2.0f };

	return position;
}

