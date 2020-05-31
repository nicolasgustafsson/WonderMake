#include "pch.h"
#include "Canvas.h"

void WmGui::BeginCanvas(SCanvasState* aCanvasState)
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

	const f32 gridSize = 64.0f * aCanvasState->ZoomLevel;

	const ImVec2 windowPosition = ImGui::GetWindowPos();
	const ImVec2 windowSize = ImGui::GetWindowSize();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

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
				ImVec2 mouseRel = ImVec2{ ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y };
				float prevZoom = aCanvasState->ZoomLevel;
				aCanvasState->ZoomLevel = std::clamp(aCanvasState->ZoomLevel + io.MouseWheel * aCanvasState->ZoomLevel / 16.f, 0.3f, 3.f);
				float zoomFactor = (prevZoom - aCanvasState->ZoomLevel) / prevZoom;

				aCanvasState->Offset.x += ImVec2((mouseRel.x - aCanvasState->Offset.x) * zoomFactor, (mouseRel.y - aCanvasState->Offset.y) * zoomFactor).x;
				aCanvasState->Offset.y += ImVec2((mouseRel.x - aCanvasState->Offset.x) * zoomFactor, (mouseRel.y - aCanvasState->Offset.y) * zoomFactor).y;
			}
		}
	}

	const ImU32 gridColor = IM_COL32(100, 100, 110, 200);

	for (f32 x = fmodf(aCanvasState->Offset.x, gridSize); x < windowSize.x;)
	{
		drawList->AddLine(ImVec2(x + windowPosition.x, 0 + windowPosition.y), ImVec2(x + windowPosition.x, windowSize.y + windowPosition.y), gridColor);
		x += gridSize;
	}

	for (f32 y = fmodf(aCanvasState->Offset.y, gridSize); y < windowSize.y;)
	{
		drawList->AddLine(ImVec2(0 + windowPosition.x, y + windowPosition.y), ImVec2(windowSize.x + windowPosition.x, y + windowPosition.y), gridColor);
		y += gridSize;
	}

	ImGui::SetWindowFontScale(aCanvasState->ZoomLevel);
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