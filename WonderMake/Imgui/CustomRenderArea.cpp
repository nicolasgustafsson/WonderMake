#include "stdafx.h"
#include "CustomRenderArea.h"

CustomRenderArea::CustomRenderArea(std::string aName)
	: myName(aName)
{

}

void CustomRenderArea::Debug()
{
	Begin();

	const SVector2f cursor_screen_pos = ImGui::GetCursorScreenPos();
	myOffset = cursor_screen_pos + myScrolling;

	DrawGrid();

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	myWindowSize = ImGui::GetWindowSize();
	// Display links
	drawList->ChannelsSplit(2);
	drawList->ChannelsSetCurrent(0); // Background

	Render();

	drawList->ChannelsMerge();

	DrawContextMenu();

	HandleScrolling();

	End();
}

void CustomRenderArea::DrawSquare(const SVector2f aPosition, const float aSize, const bool bScale, const SColor& aColor)
{
	const SVector2f transformedPoition = TransformPoint(aPosition);

	const SVector2f size = bScale ? SVector2f(aSize * myScale.X, aSize * myScale.Y) : SVector2f(aSize / 2.0f, aSize / 2.0f);
	const u32 compressedColor = ImColor(aColor.R, aColor.G, aColor.B, aColor.A);

	ImDrawList* const drawList = ImGui::GetWindowDrawList();

	drawList->AddRectFilled(transformedPoition - size, transformedPoition + size, compressedColor);
}

void CustomRenderArea::DrawLine(const SVector2f aFirst, const SVector2f aSecond, const SColor& aColor, const f32 aThickness)
{
	const u32 compressedColor = ImColor(aColor.R, aColor.G, aColor.B, aColor.A);
	ImDrawList* const drawList = ImGui::GetWindowDrawList();

	const SVector2f firstTransformed = TransformPoint(aFirst);
	const SVector2f secondTransformed = TransformPoint(aSecond);

	drawList->AddLine(firstTransformed, secondTransformed, compressedColor, aThickness);
}

void CustomRenderArea::DrawLines(const std::vector<SVector2f>& aPoints, const SColor& aColor, const f32 aThickness)
{
	ImDrawList* const drawList = ImGui::GetWindowDrawList();

	const u32 compressedColor = ImColor(aColor.R, aColor.G, aColor.B, aColor.A);

	std::vector<SVector2f> pointsTransformed = aPoints;

	for (SVector2f& point : pointsTransformed)
	{
		point = TransformPoint(point);
	}

	drawList->AddPolyline(reinterpret_cast<const ImVec2*>(pointsTransformed.data()), aPoints.size(), compressedColor, false, aThickness);
}

void CustomRenderArea::Begin()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin(myName.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImGui::End();
		return;
	}

	PreCustomArea();

	//ImGui::SameLine();
	ImGui::BeginGroup();

	// Create our child canvas
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, SVector2f(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, SVector2f(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", SVector2f(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);
}

void CustomRenderArea::End()
{
	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::End();
}

void CustomRenderArea::HandleScrolling()
{
	if (ImGui::IsWindowHovered() /*&& !ImGui::IsAnyItemActive()*/ && ImGui::IsMouseDragging(0, 0.0f))
		myScrolling = myScrolling + SVector2f(ImGui::GetIO().MouseDelta);

	const SVector2f windowSize = ImGui::GetWindowSize();
	if (myWantedPosition)
	{
		SVector2f screenCenter = (windowSize / 2.0f);

		SVector transformed = SVector2f(-myWantedPosition->X, myWantedPosition->Y);
		transformed.X *= myScale.X;
		transformed.Y *= -myScale.Y;
		myScrolling = transformed + screenCenter;
	}
	
	myWantedPosition.reset();

	const f32 scroll = ImGui::GetIO().MouseWheel;

	if (scroll != 0.f)
	{
		const SVector2f mousePos = GetMouseOnCanvas();

		myScale *= 1.0f + scroll / 10.0f;
		
		const SVector2f newMousePos = GetMouseOnCanvas();
		
		SVector2f delta = newMousePos - mousePos;
		delta.X *= myScale.X;
		delta.Y *= myScale.Y;

		myScrolling += delta;
	}
}

void CustomRenderArea::DrawContextMenu()
{
	myOpenContextMenu = false;

	// Open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		//node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		myOpenContextMenu = true;
	}
	if (myOpenContextMenu)
	{
		ImGui::OpenPopup("context_menu");
	}

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		SVector2f mouse_pos_on_opening_current_popup = ImGui::GetMousePosOnOpeningCurrentPopup();
		SVector2f scene_pos = mouse_pos_on_opening_current_popup - myOffset;

		if (ImGui::MenuItem("Add")) { /*nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2, 2));*/ }
		if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void CustomRenderArea::DrawGrid()
{
	ImDrawList* const drawList = ImGui::GetWindowDrawList();
	// Display grid
	const ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
	const float GRID_SZ = 64.0f;
	const SVector2f win_pos = ImGui::GetCursorScreenPos();
	const SVector2f canvas_sz = ImGui::GetWindowSize();

	for (float x = fmodf(myScrolling.X, GRID_SZ); x < canvas_sz.X; x += GRID_SZ)
		drawList->AddLine(SVector2f(x, 0.0f) + win_pos, SVector2f(x, canvas_sz.Y) + win_pos, GRID_COLOR);

	for (float y = fmodf(myScrolling.Y, GRID_SZ); y < canvas_sz.Y; y += GRID_SZ)
		drawList->AddLine(SVector2f(0.0f, y) + win_pos, SVector2f(canvas_sz.X, y) + win_pos, GRID_COLOR);
}

constexpr SVector2f CustomRenderArea::TransformPoint(SVector2f aPoint) const noexcept
{
	aPoint.X *= myScale.X;
	aPoint.Y *= myScale.Y;

	aPoint += myOffset;

	return aPoint;
}

SVector2f CustomRenderArea::GetMouseOnCanvas() const noexcept
{
	SVector2f mousePos = ImGui::GetMousePos();

	mousePos -= myOffset;
	mousePos.X /= myScale.X;
	mousePos.Y /= myScale.Y;

	return mousePos;
}

SVector4f CustomRenderArea::GetCanvas() const noexcept
{
	SVector2f topLeft = myScrolling;

	topLeft.X *= -1.0f;

	topLeft.X /= myScale.X;

	SVector2f scaledWindowSize = myWindowSize;
	scaledWindowSize.X /= myScale.X;
	scaledWindowSize.Y /= myScale.Y;

	SVector2f bottomRight = topLeft + scaledWindowSize;

	return { topLeft.X, topLeft.Y, bottomRight.X, bottomRight.Y };
}
