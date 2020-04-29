#include "pch.h"
#include "Canvas.h"

void WmGui::BeginCanvas(SCanvasState* aCanvasState)
{
	if (!aCanvasState)
		return;

	ImGui::PushID(aCanvasState);
	ImGui::BeginGroup();

	// Create our child canvas
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);

	const f32 grid = 64.0f * aCanvasState->ZoomLevel;

	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	f32 color = 255.f;
	ImU32 gridColor = ImColor(165.f, color, color, color);

	for (f32 x = fmodf(aCanvasState->Offset.x, grid); x < size.x;)
	{
		drawList->AddLine(ImVec2(x + pos.x, 0 + pos.y), ImVec2(x + pos.x, size.y + pos.y), gridColor);
		x += grid;
	}

	for (f32 y = fmodf(aCanvasState->Offset.y, grid); y < size.y;)
	{
		drawList->AddLine(ImVec2(0 + pos.x, y + pos.y), ImVec2(size.x + pos.x, y + pos.y), gridColor);
		y += grid;
	}

}

void WmGui::EndCanvas()
{
	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::PopID();
}
