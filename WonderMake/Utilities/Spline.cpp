#include "pch.h"
#include "Spline.h"

#include "Imgui/Canvas.h"

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

	WmGui::DrawGridOnCanvas(&canvasState, 128.f, SColor(0.1f, 0.1f, 0.1f));

	mousePos = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };

	WmGui::DrawLineOnCanvas(&canvasState, Start, ControlFirst, SColor::DimGray(0.5f), 1.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, End, ControlSecond, SColor::DimGray(0.5f), 1.f, true);
	//
	WmGui::DrawLineOnCanvas(&canvasState, { 0.0f, -1000000.f }, { 0.0f, 1000000.f }, SColor::DimGray(1.0f), 3.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, {-1000000.f, 0.0f}, { 1000000.f, 0.0f}, SColor::DimGray(1.0f), 3.f, true);

	for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	{
		SVector2f lineStart = { WmEasings::Lerp<f32>({Start.X, End.X}, progress), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, progress) };
		SVector2f lineEnd = { WmEasings::Lerp<f32>({Start.X, End.X}, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)) };
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	}

	SVector2f* closestPoint = &SVector2f::Closest(mousePos, std::array{ &Start, &End, &ControlFirst, &ControlSecond });

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			SelectedPoint = closestPoint;
		}

		if (ImGui::IsMouseDown(0) && SelectedPoint)
		{
			*SelectedPoint = mousePos;
		}
	}


	//ImDrawList* drawList = ImGui::GetWindowDrawList();
	//ImGui::GetDrawListSharedData()->Font = ImGui::GetDefaultFont();
	//ImGui::GetDrawListSharedData()->FontSize = 13.f;
	//
	//const ImVec2 windowPosition = ImGui::GetWindowPos();
	//
	////drawList->AddCircleFilled(windowPosition + ImVec2{ 200.f, 200.f }, 50.f, ImColor(SColor::White()));//, "Henlo");
	//drawList->AddText(windowPosition + ImVec2{ 200.f, 200.f }, ImColor(SColor::White()), "Henlo");
	WmGui::DrawTextOnCanvas(&canvasState, {}, "nsreitsnrie", SColor::White(), { true, false });
	WmGui::DrawTextOnCanvas(&canvasState, {}, "xcvxcvxcv", SColor::Red(), { false, true });

	WmGui::DrawCirleOnCanvas(&canvasState, Start, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, End, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, ControlFirst, SColor::ImperialRed(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, ControlSecond, SColor::ImperialRed(), 8.f, true);

	if (ImGui::IsWindowHovered())
	{
		if (!ImGui::IsMouseDown(0))
		{
			WmGui::DrawUnfilledCirleOnCanvas(&canvasState, *closestPoint, SColor::Gainsboro(), 7.f, true);
		}
	}

	if(SelectedPoint != nullptr)
	{
		WmGui::DrawCirleOnCanvas(&canvasState, *SelectedPoint, SColor::White(), 5.f, true);
		WmGui::DrawTextOnCanvas(&canvasState, *SelectedPoint, (std::string("\n  X: ") + std::to_string(SelectedPoint->X) + std::string("\n  Y: ") + std::to_string(SelectedPoint->Y)).c_str(), SColor::White(), { true, true });
		//WmGui::DrawTextOnCanvas(&canvasState, *SelectedPoint, (std::string("Y: ") + std::to_string(SelectedPoint->Y)).c_str(), SColor::White(), {true, true });

	}

	WmGui::EndCanvas();

	if (ShouldCenterEditor)
	{
		auto bb = SRectangle::GenerateBoundingBox(Start, End, ControlFirst, ControlSecond);
		canvasState.FocusRectangle(bb);
		ShouldCenterEditor = false;
	}

	return (earlier != *this) ? EAlterStatus::Changed : EAlterStatus::Same;
}
