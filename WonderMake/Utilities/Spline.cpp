#include "pch.h"
#include "Spline.h"

#include "Imgui/Canvas.h"

EAlterStatus FloatSpline::Inspect()
{
	const FloatSpline earlier = *this;

	static SVector2f mousePos;
	static WmGui::SCanvasState canvasState;

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
	canvasState.ScaleToRectangle(SRectangle::GenerateBoundingBox(Start, End, ControlFirst, ControlSecond));

	WmGui::BeginCanvas(&canvasState, true, true);
	mousePos = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };

	WmGui::DrawLineOnCanvas(&canvasState, Start, ControlFirst, SColor::DimGray(0.5f), 1.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, End, ControlSecond, SColor::DimGray(0.5f), 1.f, true);

	WmGui::DrawLineOnCanvas(&canvasState, { 0.0f, -1000000.f }, { 0.0f, 1000000.f }, SColor::DimGray(1.0f), 3.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, {-1000000.f, 0.0f}, { 1000000.f, 0.0f}, SColor::DimGray(1.0f), 3.f, true);

	for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	{
		SVector2f lineStart = { WmEasings::Lerp<f32>({Start.X, End.X}, progress), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, progress) };
		SVector2f lineEnd = { WmEasings::Lerp<f32>({Start.X, End.X}, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)) };
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	}


	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			SelectedPoint = &SVector2f::Closest(mousePos, std::array{ &Start, &End, &ControlFirst, &ControlSecond });
		}

		if (ImGui::IsMouseDown(0) && SelectedPoint)
		{
			*SelectedPoint = mousePos;
		}
	}

	WmGui::DrawCirleOnCanvas(&canvasState, Start, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, End, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, ControlFirst, SColor::ImperialRed(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, ControlSecond, SColor::ImperialRed(), 8.f, true);

	if(SelectedPoint != nullptr)
		WmGui::DrawCirleOnCanvas(&canvasState, *SelectedPoint, SColor::White(), 5.f, true);

	WmGui::EndCanvas();

	return (earlier != *this) ? EAlterStatus::Changed : EAlterStatus::Same;
}
