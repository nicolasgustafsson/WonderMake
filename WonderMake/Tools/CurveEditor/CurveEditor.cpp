#include "stdafx.h"
#include "CurveEditor.h"
#include "Utilities/Curve/Curve.h"

CurveEditor::CurveEditor()
	:CustomRenderArea(std::string("Curve Editor"))
{
	//myCurve.myStart = SVector2f::Zero();
	//myCurve.myEnd = { 100.0f, 0.0f };

	myCurve.myStart.myPoint = SVector2f::Zero();
	myCurve.myStart.myControlPoint = { 25.f, 25.f };
	myCurve.myEnd.myPoint = {100.f, 0.f};
	myCurve.myEnd.myControlPoint = { 75.f, 25.f };

	myScale.Y = -1.0f;

	myScrolling += {200.0f, 300.0f};
}

void CurveEditor::PreCustomArea() noexcept
{
	ImGui::SameLine();
	if (ImGui::Button("Center"))
	{
		SetCenter(myCurve.GetCenter());
	}
}

void CurveEditor::Render()
{
	SVector2f center = myCurve.GetCenter();
	//myCurve.myStart = SVector2f::Zero();
	//myCurve.myEnd = { 100.0f, 0.0f };
	//myCurve.myControlPoint = { 50.0f, 100.0f };

	DrawLine({ 0.0f, 1000000000000.0f }, { 0.0f, -1000000000000.0f }, { 0.7f, 0.7f, 0.7f, 1.0f }, 2.0f);
	DrawLine({ -1000000000000.0f, 0.0f }, { 1000000000000.0f, 0.0f }, { 0.7f, 0.7f, 0.7f, 1.0f }, 2.0f);
	DrawLines(myCurve.GetPoints(GetCanvas()), { 0.8f, 0.8f, 0.8f, 1.0f }, 3.0f);

	//DoDragPoint(myCurve.myStart.myPoint, 10.0f);
	//DoDragPoint(myCurve.myStart.myControlPoint, 6.0f);
	//DoDragPoint(myCurve.myEnd.myPoint, 10.0f);
	//DoDragPoint(myCurve.myEnd.myControlPoint, 6.0f);

	DoCurvePoint(myCurve.myStart);
	DoCurvePoint(myCurve.myEnd);

	//DrawSquare(myCurve.GetCenter(), 4.0f, false, SColor::Green);
	PostProcessPoints();
}

void CurveEditor::DoDragPoint(SVector2f& aPoint, const f32 aSize)
{
	ImGui::PushID(&aPoint);

	ImGui::SetCursorScreenPos(TransformPoint(aPoint) - SVector2f(aSize / 2.f + 4.0f, aSize / 2.f + 4.0f));

	ImGui::InvisibleButton("", SVector2f(aSize + 8.0f, aSize + 8.0f));
	if (ImGui::IsItemActive())
	{
		if (ImGui::IsMouseDragging(0, 0.f))
		{
			aPoint += {ImGui::GetIO().MouseDelta.x / myScale.X, ImGui::GetIO().MouseDelta.y / myScale.Y};
		}

		DrawSquare(aPoint, aSize, false, SColor::Yellow);
	}
	else
	{
		DrawSquare(aPoint, aSize, false, SColor::White);
	}

	ImGui::PopID();
}

void CurveEditor::DoCurvePoint(SCurvePoint& aPoint)
{
	ImGui::PushID(&aPoint.myPoint);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	drawList->ChannelsSetCurrent(1);
	ImGui::SetCursorScreenPos(TransformPoint(aPoint.myPoint) - SVector2f(10.0f / 2.f + 4.0f, 10.0f / 2.f + 4.0f));
	ImGui::InvisibleButton("", SVector2f(10.0f + 8.0f, 10.0f + 8.0f));
	if (ImGui::IsItemActive())
	{
		if (ImGui::IsMouseDragging(0, 0.f))
		{
			aPoint.myPoint += {ImGui::GetIO().MouseDelta.x / myScale.X, ImGui::GetIO().MouseDelta.y / myScale.Y};
			aPoint.myControlPoint += {ImGui::GetIO().MouseDelta.x / myScale.X, ImGui::GetIO().MouseDelta.y / myScale.Y};
		}
	}

	SColor PointColor = ImGui::IsItemActive() ? SColor::Yellow : ImGui::IsItemHovered() ? SColor::White : SColor({ 0.8f, 0.8f, 0.8f, 1.0f } );
	DrawSquare(aPoint.myPoint, 10.0f, false, PointColor);

	ImGui::PopID();
	drawList->ChannelsSetCurrent(0);

	ImGui::SetCursorScreenPos(TransformPoint(aPoint.myControlPoint) - SVector2f(6.0f / 2.f + 4.0f, 6.0f / 2.f + 4.0f));
	ImGui::PushID(&aPoint.myControlPoint);
	ImGui::InvisibleButton("", SVector2f(6.0f + 8.0f, 6.0f + 8.0f));
	PointColor = ImGui::IsItemActive() ? SColor::Yellow : ImGui::IsItemHovered() ? SColor::White : SColor({ 0.8f, 0.8f, 0.8f, 1.0f });
	DrawSquare(aPoint.myControlPoint, 6.0f, false, PointColor);

	if (ImGui::IsItemActive())
	{
		if (ImGui::IsMouseDragging(0, 0.f))
		{
			aPoint.myControlPoint += {ImGui::GetIO().MouseDelta.x / myScale.X, ImGui::GetIO().MouseDelta.y / myScale.Y};
		}

		DrawLine(aPoint.myPoint, aPoint.myControlPoint, SColor::Yellow);
	}
	else
	{
		DrawLine(aPoint.myPoint, aPoint.myControlPoint, { 0.8f, 0.8f, 0.8f, 1.0f });
	}
	ImGui::PopID();
}

void CurveEditor::PostProcessPoints()
{
	if (myCurve.myStart.myPoint.X > myCurve.myEnd.myPoint.X)
	{
		std::swap(myCurve.myEnd, myCurve.myStart);
	}
}
