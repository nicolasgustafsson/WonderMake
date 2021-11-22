#include "pch.h"
#include "Canvas.h"

void WmGui::SCanvasState::FocusRectangle(const SRectangle& aRectangle)
{
	SRectangle rect = aRectangle;

	rect.ExpandMultiplicative(1.2f);
	
	ZoomLevel.x = (WindowSize.x) / (rect.GetWidth());
	ZoomLevel.y = (WindowSize.y) / (rect.GetHeight());

	if (InvertY)
		ZoomLevel.y *= -1.f;

	if (!InvertY)
		Offset = { -rect.Left * ZoomLevel.x, -rect.Bottom * ZoomLevel.y };
	else
		Offset = { -rect.Left * ZoomLevel.x, -rect.Top * ZoomLevel.y };

}

void WmGui::BeginCanvas(SCanvasState* aCanvasState, bool aAllowMovement /*= true*/, bool aShowGrid /*= true*/)
{
	assert(aCanvasState != nullptr);

	ImGui::PushID(aCanvasState);
	IdCount++;
	
	ImGui::BeginGroup();

	// Create our child canvas
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(55, 55, 60, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);

	const ImVec2 windowPosition = ImGui::GetWindowPos();

	if (aAllowMovement)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (!ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			if (ImGui::IsMouseDragging(2))
			{
				aCanvasState->Offset.x += io.MouseDelta.x;
				aCanvasState->Offset.y += io.MouseDelta.y;
			}

			if (io.KeyCtrl)
			{
				if (io.MouseWheel != 0)
				{
					ImVec2 mouseRel = ImVec2{ ImGui::GetMousePos().x - windowPosition.x, ImGui::GetMousePos().y - windowPosition.y };
					ImVec2 prevZoom = aCanvasState->ZoomLevel;
					aCanvasState->ZoomLevel = { std::clamp(aCanvasState->ZoomLevel.x + io.MouseWheel * aCanvasState->ZoomLevel.x / 16.f, -30000.f, 30000.f), std::clamp(aCanvasState->ZoomLevel.y + io.MouseWheel * aCanvasState->ZoomLevel.y / 16.f, -30000.f, 30000.f) };
					ImVec2 zoomFactor = (prevZoom - aCanvasState->ZoomLevel) / prevZoom;

					aCanvasState->Offset.x += ImVec2((mouseRel.x - aCanvasState->Offset.x) * zoomFactor.x, (mouseRel.y - aCanvasState->Offset.y) * zoomFactor.x).x;
					aCanvasState->Offset.y += ImVec2((mouseRel.x - aCanvasState->Offset.x) * zoomFactor.y, (mouseRel.y - aCanvasState->Offset.y) * zoomFactor.y).y;
				}
			}
		}
	}

	const ImVec2 windowSize = ImGui::GetWindowSize();

	aCanvasState->WindowSize = windowSize;

	if (aShowGrid)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 gridSize = aCanvasState->ZoomLevel * 64.0f;
		const ImU32 gridColor = IM_COL32(100, 100, 110, 200);

		for (f32 x = fmodf(aCanvasState->Offset.x, gridSize.x); x < windowSize.x;)
		{
			drawList->AddLine(ImVec2(x + windowPosition.x, 0 + windowPosition.y), ImVec2(x + windowPosition.x, windowSize.y + windowPosition.y), gridColor);
			x += gridSize.x;
		}

		for (f32 y = fmodf(aCanvasState->Offset.y, gridSize.y); y < windowSize.y;)
		{
			drawList->AddLine(ImVec2(0 + windowPosition.x, y + windowPosition.y), ImVec2(windowSize.x + windowPosition.x, y + windowPosition.y), gridColor);
			y += gridSize.y;
		}
	}

	ImGui::SetWindowFontScale(aCanvasState->ZoomLevel.x);
}

ImVec2 WmGui::GetMousePosOnCanvas(SCanvasState* aCanvasState)
{
	assert(aCanvasState);
	return (ImGui::GetMousePos() - ImGui::GetWindowPos() - aCanvasState->Offset) / aCanvasState->ZoomLevel;
}

ImVec2 WmGui::GetPositionOnWindow(SCanvasState* aCanvasState, SVector2f aCanvasPosition)
{
	return { aCanvasPosition.X * aCanvasState->ZoomLevel.x, aCanvasPosition.Y * aCanvasState->ZoomLevel.y };
}

void WmGui::DrawCirleOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const SColor aColor, const f32 aRadius, const bool aHandleOffset)
{
	assert(aCanvasState);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 windowPosition = ImGui::GetWindowPos();

	const ImVec2 offset = aHandleOffset ? windowPosition + aCanvasState->Offset : windowPosition;

	//drawList->AddLine(ImVec2(x + windowPosition.x, 0 + windowPosition.y), ImVec2(x + windowPosition.x, windowSize.y + windowPosition.y), gridColor);
	drawList->AddCircleFilled(offset + ImVec2{ aPosition.X, aPosition.Y } *aCanvasState->ZoomLevel, aRadius,
		ImColor(aColor.R, aColor.G, aColor.B, aColor.A));

}

void WmGui::DrawUnfilledCirleOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const SColor aColor,
	const f32 aRadius, bool aHandleOffset)
{

	assert(aCanvasState);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 windowPosition = ImGui::GetWindowPos();

	const ImVec2 offset = aHandleOffset ? windowPosition + aCanvasState->Offset : windowPosition;

	//drawList->AddLine(ImVec2(x + windowPosition.x, 0 + windowPosition.y), ImVec2(x + windowPosition.x, windowSize.y + windowPosition.y), gridColor);
	drawList->AddCircle(offset + ImVec2{ aPosition.X, aPosition.Y } *aCanvasState->ZoomLevel, aRadius,
		ImColor(aColor.R, aColor.G, aColor.B, aColor.A), 0, 2.f);
}

void WmGui::DrawLineOnCanvas(SCanvasState* aCanvasState, const SVector2f aStart, const SVector2f aEnd, const SColor aColor, const f32 aThickness, bool aHandleOffset)
{
	assert(aCanvasState);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 windowPosition = ImGui::GetWindowPos();

	const ImVec2 offset = aHandleOffset ? windowPosition + aCanvasState->Offset : windowPosition;

	drawList->AddLine(offset + ImVec2{ aStart.X, aStart.Y } * aCanvasState->ZoomLevel, offset + ImVec2{ aEnd.X, aEnd.Y } * aCanvasState->ZoomLevel, ImColor(aColor.R, aColor.G, aColor.B, aColor.A), aThickness);
}

void WmGui::DrawGridOnCanvas(SCanvasState* aCanvasState, const f32 aGridSize, const SColor aColor)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 gridSize = { aGridSize, aGridSize };

	const ImVec2 windowPosition = ImGui::GetWindowPos();
	const ImVec2 windowSize = ImGui::GetWindowSize();

	for (f32 x = fmodf(aCanvasState->Offset.x, gridSize.x); x < windowSize.x;)
	{
		drawList->AddLine(ImVec2(x + windowPosition.x, 0 + windowPosition.y), ImVec2(x + windowPosition.x, windowSize.y + windowPosition.y), ImColor(aColor.R, aColor.G, aColor.B, aColor.A));
		x += gridSize.x;
	}

	for (f32 y = fmodf(aCanvasState->Offset.y, gridSize.y); y < windowSize.y;)
	{
		drawList->AddLine(ImVec2(0 + windowPosition.x, y + windowPosition.y), ImVec2(windowSize.x + windowPosition.x, y + windowPosition.y), ImColor(aColor.R, aColor.G, aColor.B, aColor.A));
		y += gridSize.y;
	}
}

void WmGui::DrawTextOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const char* aText, const SColor aColor, const SVector2<bool> aHandleOffset)
{
	aCanvasState;
	aHandleOffset;
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImGui::GetDrawListSharedData()->Font = ImGui::GetDefaultFont();
	ImGui::GetDrawListSharedData()->FontSize = 13.f;
	const ImVec2 windowPosition = ImGui::GetWindowPos();

	const ImVec2 offset = { aHandleOffset.X ? windowPosition.x + aCanvasState->Offset.x : windowPosition.x, aHandleOffset.Y ? windowPosition.y + aCanvasState->Offset.y : windowPosition.y };


	//drawList->AddCircleFilled(windowPosition + ImVec2{ 200.f, 200.f }, 50.f, ImColor(SColor::White()));//, "Henlo");
	drawList->AddText(offset + ImVec2{aPosition.X, aPosition.Y} *aCanvasState->ZoomLevel, ImColor(aColor), aText);
}

void WmGui::EndCanvas()
{
	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::SetWindowFontScale(1.f);
	ImGui::PopID();
	IdCount--;
}
