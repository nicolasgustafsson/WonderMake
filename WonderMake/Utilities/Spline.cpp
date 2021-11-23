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

	const f32 gridSize = 128.f;
	WmGui::DrawGridOnCanvas(&canvasState, 128.f, SColor(0.1f, 0.1f, 0.1f));

	mousePos = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };

	WmGui::DrawLineOnCanvas(&canvasState, Start, ControlFirst, SColor::DimGray(0.5f), 1.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, End, ControlSecond, SColor::DimGray(0.5f), 1.f, true);
	//WmGui::DrawLineOnCanvas(&canvasState, Third, ControlThird, SColor::DimGray(0.5f), 1.f, true);
	//
	WmGui::DrawLineOnCanvas(&canvasState, { 0.0f, -1000000.f }, { 0.0f, 1000000.f }, SColor::DimGray(1.0f), 3.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, {-1000000.f, 0.0f}, { 1000000.f, 0.0f}, SColor::DimGray(1.0f), 3.f, true);

	for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	{
		SVector2f lineStart = { WmEasings::Lerp<f32>({Start.X, End.X}, progress), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, progress) };
		SVector2f lineEnd = { WmEasings::Lerp<f32>({Start.X, End.X}, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)) };
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	}

	for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	{
		SVector2f lineStart = { WmEasings::Lerp<f32>({Start.X, End.X}, progress), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, progress) };
		SVector2f lineEnd = { WmEasings::Lerp<f32>({Start.X, End.X}, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)) };
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	}

	const ImVec2 windowSize = ImGui::GetWindowSize();
	for (f32 x = fmodf(canvasState.Offset.x, gridSize) - gridSize; x < windowSize.x;)
	{
		const f32 position = (x / canvasState.ZoomLevel.x) - canvasState.Offset.x / canvasState.ZoomLevel.x;
		const f32 position2 = ((x + 20.f) / canvasState.ZoomLevel.x) - canvasState.Offset.x / canvasState.ZoomLevel.x;

		const f32 relativePosition = WmEasings::InvLerp(SRange{ Start.X, End.X }, position);
		const f32 relativePosition2 = WmEasings::InvLerp(SRange{ Start.X, End.X }, position2);

		SVector2f lineStart = { position, WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, relativePosition) };
		SVector2f lineEnd = { position2, WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, relativePosition2) };

		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::MinionYellow(), 2.f, true);

		x += 20.f;

		//drawList->AddText(ImVec2(x + windowPosition.x + 5, windowSize.y + windowPosition.y - 15), ImColor(255, 0, 0, 255), std::to_string(position).c_str());
		//drawList->AddText(ImVec2(x + windowPosition.x + 5, 0 + windowPosition.y), ImColor(255, 0, 0, 255), std::to_string(position).c_str());
		//x += gridSize;
	}

	//for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	//{
	//	SVector2f lineStart = { WmEasings::Lerp<f32>({End.X, Third.X}, progress), WmEasings::CubicBezier({ End.Y, Third.Y }, ControlSecond.Y, ControlThird.Y, progress) };
	//	SVector2f lineEnd = { WmEasings::Lerp<f32>({End.X, Third.X}, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)), WmEasings::CubicBezier({ End.Y, Third.Y }, ControlSecond.Y, ControlThird.Y, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)) };
	//	WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	//}
	

	for (f32 progress = -10.f; progress < 0.0f; progress += 0.05f)
	{
		SVector2f lineStart = { WmEasings::Lerp<f32>({Start.X, End.X}, progress), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, progress) };
		SVector2f lineEnd = { WmEasings::Lerp<f32>({Start.X, End.X}, WmMath::Clamp(-10.f, 0.0f, progress + 0.025f)), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, WmMath::Clamp(-10.f, 0.0f, progress + 0.025f)) };
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::DimGray(), 1.f, true);
	}

	//for (f32 progress = 1.f; progress < 10.0f; progress += 0.05f)
	//{
	//	SVector2f lineStart = { WmEasings::Lerp<f32>({Start.X, End.X}, progress), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, progress) };
	//	SVector2f lineEnd = { WmEasings::Lerp<f32>({Start.X, End.X}, WmMath::Clamp(1.f, 10.0f, progress + 0.025f)), WmEasings::CubicBezier({ Start.Y, End.Y }, ControlFirst.Y, ControlSecond.Y, WmMath::Clamp(1.f, 10.0f, progress + 0.025f)) };
	//	WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::DimGray(), 1.f, true);
	//}

	std::vector<SVector2f*> positions = { &Start, &End, &ControlFirst, &ControlSecond };

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

	
	WmGui::DrawCirleOnCanvas(&canvasState, Start, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, End, SColor::BlueBell(), 8.f, true);
	//WmGui::DrawCirleOnCanvas(&canvasState, Third, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, ControlFirst, SColor::ImperialRed(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, ControlSecond, SColor::ImperialRed(), 8.f, true);
	//WmGui::DrawCirleOnCanvas(&canvasState, ControlThird, SColor::ImperialRed(), 8.f, true);

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
		//WmGui::DrawTextOnCanvas(&canvasState, *SelectedPoint, (std::string("Y: ") + std::to_string(SelectedPoint->Y)).c_str(), SColor::White(), {true, true });

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
		auto bb = SRectangle::GenerateBoundingBox(Start, End, ControlFirst, ControlSecond);
		canvasState.FocusRectangle(bb);
		ShouldCenterEditor = false;
	}

	return (earlier != *this) ? EAlterStatus::Changed : EAlterStatus::Same;
}
