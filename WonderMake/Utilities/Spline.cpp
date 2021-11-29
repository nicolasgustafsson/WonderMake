#include "pch.h"
#include "Spline.h"

#include "Imgui/Canvas.h"

f32 FloatSpline::GetValue(const f32 aX)
{
	auto pointCombo = GetPointCombo(aX);
	const f32 relativePosition = WmEasings::InvLerp(SRange{ pointCombo.first.Point.X, pointCombo.second.Point.X }, aX);
	return WmEasings::CubicBezier({ pointCombo.first.Point.Y, pointCombo.second.Point.Y }, pointCombo.first.Control.Y, pointCombo.second.ControlInverse.Y, relativePosition);
}

EAlterStatus FloatSpline::Inspect()
{
	const FloatSpline earlier = *this;

	static SVector2f mousePos;
	static WmGui::SCanvasState canvasState;
	canvasState.InvertY = true;

	if (SelectedPoint)
	{
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat("X", &(SelectedPoint->X));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);

		ImGui::InputFloat("Y", &(SelectedPoint->Y));
	}
	else
	{
		static SVector2f nullVec{};
		ImGui::BeginDisabled();
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat("X", &(nullVec.X));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat("Y", &(nullVec.Y));
		ImGui::EndDisabled();
	}

	ImGui::SameLine();

	if (ImGui::Button("Center"))
	{
		ShouldCenterEditor = true;
	}

	WmGui::BeginCanvas(&canvasState, true, false);

	const f32 gridSize = 128.f;
	WmGui::DrawGridOnCanvas(&canvasState, 128.f, SColor(0.1f, 0.1f, 0.1f));

	mousePos = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };
	for(auto&& point : Points)
	{
		WmGui::DrawLineOnCanvas(&canvasState, point.Point, point.Control, SColor::DimGray(0.5f), 1.f, true);
	}
	
	//
	WmGui::DrawLineOnCanvas(&canvasState, { 0.0f, -1000000.f }, { 0.0f, 1000000.f }, SColor::DimGray(1.0f), 3.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, {-1000000.f, 0.0f}, { 1000000.f, 0.0f}, SColor::DimGray(1.0f), 3.f, true);

	const ImVec2 windowSize = ImGui::GetWindowSize();
	for (f32 x = fmodf(canvasState.Offset.x, gridSize) - gridSize; x < windowSize.x;)
	{
		const f32 nextInteresting = GetNextInterestingScreenX(x);

		const f32 position = (x / canvasState.ZoomLevel.x) - canvasState.Offset.x / canvasState.ZoomLevel.x;
		const f32 position2 = (nextInteresting / canvasState.ZoomLevel.x) - canvasState.Offset.x / canvasState.ZoomLevel.x;

		WmGui::DrawLineOnCanvas(&canvasState, { position, GetValue(position)}, { position2, GetValue(position2) }, SColor::MinionYellow(), 2.f, true);

		x = nextInteresting;
	}

	std::vector<SVector2f*> positions;
	positions.reserve(Points.size() * 2);

	for(auto& point : Points)
	{
		positions.push_back(&point.Point);
		positions.push_back(&point.Control);
	}

	SVector2f* closestPoint = nullptr;
	f32 distance = 10000000.f;

	{
		std::vector<SVector2f> convertedPositions;

		for (SVector2f* position : positions)
		{
			convertedPositions.push_back(WmGui::ConvertToScreenPosition(&canvasState, *position));
		}
		const SVector2f mouseScreenPos{ (ImGui::GetMousePos()).x, (ImGui::GetMousePos()).y };
		const size_t closestIndex = SVector2f::ClosestIndex(mouseScreenPos, convertedPositions);
		closestPoint = positions[closestIndex];

		distance = mouseScreenPos.DistanceTo(convertedPositions[closestIndex]);
	}

	const bool hoveringAnyPoint = distance < 30.f;

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			if (hoveringAnyPoint)
				SelectedPoint = closestPoint;
			else
				SelectedPoint = nullptr;

		}

		if (ImGui::IsMouseDown(0) && SelectedPoint)
		{
			*SelectedPoint = mousePos;
		}
	}


	for(i32 i = 0; i < Points.size() - 1; i++)
	{
		if (Points[i].Point.X > Points[i+1].Point.X)
		{
			std::swap(Points[i], Points[i + 1]);

			if (SelectedPoint == &Points[i].Point)
				SelectedPoint = &Points[i + 1].Point;
			else if (SelectedPoint == &Points[i + 1].Point)
				SelectedPoint = &Points[i].Point;
		}
	}

	for (auto&& point : Points)
	{

		point.ControlInverse = point.Point - (point.Control - point.Point);
		WmGui::DrawCirleOnCanvas(&canvasState, point.Point, SColor::BlueBell(), 8.f, true);
		WmGui::DrawCirleOnCanvas(&canvasState, point.Control, SColor::ImperialRed(), 8.f, true);
		//WmGui::DrawCirleOnCanvas(&canvasState, point.ControlInverse, SColor::ImperialRed(), 8.f, true);
	}

	if (ImGui::IsWindowHovered())
	{
		if (!ImGui::IsMouseDown(0) && hoveringAnyPoint)
		{
			WmGui::DrawUnfilledCirleOnCanvas(&canvasState, *closestPoint, SColor::Gainsboro(), 7.f, true);
		}
	}

	if(SelectedPoint != nullptr)
	{
		WmGui::DrawCirleOnCanvas(&canvasState, *SelectedPoint, SColor::White(), 5.f, true);
		WmGui::DrawTextOnCanvas(&canvasState, *SelectedPoint, (std::string("\n  X: ") + std::to_string(SelectedPoint->X) + std::string("\n  Y: ") + std::to_string(SelectedPoint->Y)).c_str(), SColor::White(), { true, true });

	}

	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const ImVec2 windowPosition = ImGui::GetWindowPos();

		ImGui::GetDrawListSharedData()->Font = ImGui::GetDefaultFont();
		ImGui::GetDrawListSharedData()->FontSize = 13.f;

		for (f32 x = fmodf(canvasState.Offset.x, gridSize); x < windowSize.x;)
		{
			const f32 position = (x / canvasState.ZoomLevel.x) - canvasState.Offset.x / canvasState.ZoomLevel.x;

			drawList->AddText(ImVec2(x + windowPosition.x + 5, windowSize.y + windowPosition.y - 15), ImColor(255, 0, 0, 255), std::to_string(position).c_str());
			drawList->AddText(ImVec2(x + windowPosition.x + 5, 0 + windowPosition.y), ImColor(255, 0, 0, 255), std::to_string(position).c_str());
			x += gridSize;
		}

		for (f32 y = fmodf(canvasState.Offset.y, gridSize); y < windowSize.y;)
		{
			const f32 position = (y / canvasState.ZoomLevel.y) - canvasState.Offset.y / canvasState.ZoomLevel.y;
			drawList->AddText(ImVec2(windowSize.x + windowPosition.x - 30.f, y + windowPosition.y), ImColor(255, 255, 255, 255), std::to_string(position).c_str());
			drawList->AddText(ImVec2(windowPosition.x, y + windowPosition.y), ImColor(255, 255, 255, 255), std::to_string(position).c_str());
			y += gridSize;
		}
	}

	WmGui::EndCanvas();

	if (ShouldCenterEditor)
	{
		std::vector<SVector2f> points;
		points.reserve(Points.size() * 2);

		for(auto&& point : Points)
		{
			points.push_back(point.Point);
			points.push_back(point.Control);
		}

		auto bb = SRectangle::GenerateBoundingBox(points);
		canvasState.FocusRectangle(bb);
		ShouldCenterEditor = false;
	}

	return (earlier != *this) ? EAlterStatus::Changed : EAlterStatus::Same;
}

f32 FloatSpline::GetNextInterestingScreenX(const f32 aX)
{
	//Nicos this is not the best way but it works for my purposes; it might cause too many lines(1 per 2 pixels)
	return aX + 2.f;
}

std::pair<FloatSpline::SSplinePoint&, FloatSpline::SSplinePoint&> FloatSpline::GetPointCombo(const f32 aX)
{
	for(i32 i = 0; i < Points.size() - 1; i++)
	{
		SSplinePoint& point = Points[i];
		SSplinePoint& nextPoint = Points[i+1];

		if (point.Point.X < aX && nextPoint.Point.X > aX)
			return { point, nextPoint };

		if (nextPoint.Point.X < aX && i == Points.size() - 2)
			return { point, nextPoint };
	}

	return { Points[0], Points[1] };
}
