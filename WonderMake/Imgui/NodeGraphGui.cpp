#include "pch.h"
#include "NodeGraphGui.h"
#include "NodeGraph/NodeGraph.h"
#include "NodeGraph/Node.h"

static WmGui::SCanvasState* CurrentCanvasState;

/// Struct containing information about connection source node and slot.
struct _DragConnectionPayload
{
	/// Node id where connection started.
	void* NodeId = nullptr;
	/// Source slot name.
	const char* SlotTitle = nullptr;
	/// Source slot kind.
	bool IsInput = 0;

	SInputSlotInstanceBase* InputSlotInstance;
	SOutputSlotInstanceBase* OutputSlotInstance;

	ImColor SlotColor;
};

void WmGui::NodeGraphEditor(NodeGraph& aNodeGraph, bool* aShouldShow)
{
	ImGui::Begin(aNodeGraph.Name.c_str(), aShouldShow);

	WmGui::BeginCanvas(&aNodeGraph.CanvasState);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	drawList->ChannelsSplit(4);

	WmGui::Nodes(aNodeGraph.Nodes);

	drawList->ChannelsSetCurrent(0);

	WmGui::Connections(aNodeGraph.Connections);

	WmGui::PotentialConnection(aNodeGraph);

	WmGui::DrawPendingConnection();

	drawList->ChannelsSetCurrent(3);
	WmGui::ContextMenu(aNodeGraph);

	drawList->ChannelsMerge();
	WmGui::EndCanvas();

	ImGui::End();
}

ImU32 WmGui::MakeSlotDataID(const char* aData, const char* aSlotTitle, void* aNodeId, bool aIsInput)
{
	ImU32 SlotId = ImHashStr(aSlotTitle, 0, ImHashData(&aNodeId, sizeof(aNodeId)));
	if (aIsInput)
	{
		// Ensure that input and output slots with same name have different ids.
		SlotId ^= ~0U;
	}
	return ImHashStr(aData, 0, SlotId);
}

bool WmGui::RenderConnection(const ImVec2& aInputPosition, const ImVec2& aOutputPosition, float aThickness, const ImColor color)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	aThickness *= CurrentCanvasState->ZoomLevel;

	ImVec2 closestPoint = ImLineClosestPoint(aInputPosition, aOutputPosition, ImGui::GetMousePos());
	float minimumSquareDistance = ImFabs(ImLengthSqr(ImGui::GetMousePos() - closestPoint));
	bool isClose = minimumSquareDistance <= 200;

	drawList->AddLine(aInputPosition, aOutputPosition, color, isClose ? aThickness * 2.f : aThickness);

	if(isClose)
		drawList->AddLine(aInputPosition, aOutputPosition, IM_COL32(255, 255, 255, 100), aThickness * 2.f);

	return isClose;
}

bool WmGui::GetNewConnection(void** aInputNodeId, const char** aInputTitle, void** aOutputNodeId, const char** aOutputTitle, ImColor* aColor, SInputSlotInstanceBase** aInputSlotInstance, SOutputSlotInstanceBase** aOutputSlotInstance)
{
	assert(CurrentCanvasState != nullptr);
	assert(aInputNodeId != nullptr);
	assert(aInputTitle != nullptr);
	assert(aOutputNodeId != nullptr);
	assert(aOutputTitle != nullptr);
	assert(aColor != nullptr);

	auto* canvas = CurrentCanvasState;
	auto& nodeGraphState = canvas->NodeGraphState;

	if (nodeGraphState.NewConnection.OutputNodeId != nullptr)
	{
		*aInputNodeId = CurrentCanvasState->NodeGraphState.NewConnection.InputNodeId;
		*aInputTitle = CurrentCanvasState->NodeGraphState.NewConnection.InputSlotName;
		*aOutputNodeId = CurrentCanvasState->NodeGraphState.NewConnection.OutputNodeId;
		*aOutputTitle = CurrentCanvasState->NodeGraphState.NewConnection.OutputSlotName;
		*aInputSlotInstance = CurrentCanvasState->NodeGraphState.NewConnection.InputSlot;
		*aOutputSlotInstance = CurrentCanvasState->NodeGraphState.NewConnection.OutputSlot;
		*aColor = nodeGraphState.NewConnection.Color;

		memset(&nodeGraphState.NewConnection, 0, sizeof(nodeGraphState.NewConnection));
		return true;
	}

	return false;
}

bool WmGui::Connection(void* aInputNode, const char* aInputSlot, void* aOutputNode, const char* aOutputSlot, ImColor color)
{
	assert(CurrentCanvasState != nullptr);
	assert(aInputNode != nullptr);
	assert(aInputSlot != nullptr);
	assert(aOutputNode != nullptr);
	assert(aOutputSlot != nullptr);

	bool isConnected = true;
	auto* canvas = CurrentCanvasState;

	ImVec2 input_slot_pos{
		canvas->CachedData.GetFloat(MakeSlotDataID("x", aInputSlot, aInputNode, true)),
		canvas->CachedData.GetFloat(MakeSlotDataID("y", aInputSlot, aInputNode, true)),
	};

	ImVec2 output_slot_pos{
		canvas->CachedData.GetFloat(MakeSlotDataID("x", aOutputSlot, aOutputNode, false)),
		canvas->CachedData.GetFloat(MakeSlotDataID("y", aOutputSlot, aOutputNode, false)),
	};

	// Indent connection a bit into slot widget.
	//const float indent =  5.0f * canvas->ZoomLevel;
	//input_slot_pos.x += indent;
	//output_slot_pos.x -= indent;

	bool isCurveHovered = RenderConnection(input_slot_pos, output_slot_pos, 2.0f, color);
	if (isCurveHovered && ImGui::IsWindowHovered())
	{
		ImGuiIO& io = ImGui::GetIO();

		if ((ImGui::IsMouseClicked(0) && io.KeyAlt) || ImGui::IsMouseClicked(2))
			isConnected = false;
	}

	canvas->CachedData.SetBool(MakeSlotDataID("hovered", aInputSlot, aInputNode, true), isCurveHovered && isConnected);
	canvas->CachedData.SetBool(MakeSlotDataID("hovered", aOutputSlot, aOutputNode, false), isCurveHovered && isConnected);

	return isConnected;
}

void WmGui::BeginCanvas(SCanvasState* aCanvasState)
{
	assert(aCanvasState != nullptr);

	CurrentCanvasState = aCanvasState;
	ImGui::PushID(aCanvasState);
	ImGui::BeginGroup();

	// Create our child canvas
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(55, 55, 60, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushItemWidth(120.0f);

	const f32 gridSize = 64.0f * aCanvasState->ZoomLevel;

	ImVec2 windowPosition = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto& imguiStyle = ImGui::GetStyle();

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

	ImU32 gridColor = IM_COL32(100, 100, 110, 200);

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

	ImGui::SetWindowFontScale(CurrentCanvasState->ZoomLevel);
}

void WmGui::Node(SNode& node)
{
	WmGui::BeginNode(&node, &node.Position, &node.Selected);
	WmGui::NodeTitle(node.NodeType.Title.c_str());

	WmGui::InputSlots(node.InputSlotInstances);

	//ImGui::Text("Testing 123"); //[Nicos]: TODO add a way to preview stuff here - depending on compiler

	WmGui::OutputSlots(node.OutputSlotInstances);
	WmGui::EndNode();
}

void WmGui::EndCanvas()
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	
	if (CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DoSelectionsFrame <= ImGui::GetCurrentContext()->FrameCount)
		CurrentCanvasState->NodeGraphState.CurrentGraphInfo.SingleSelectedNodeId = nullptr;

	switch (CurrentCanvasState->NodeGraphState.CurrentGraphInfo.CurrentCursorState)
	{
	case SNodeGraphState::CursorState::None:
	{
		ImGuiID canvasId = ImGui::GetID("canvas");
		if (ImGui::IsMouseDown(0) && ImGui::GetCurrentWindow()->ContentRegionRect.Contains(ImGui::GetMousePos()))
		{
			if (ImGui::IsWindowHovered())
			{
				if (!ImGui::IsWindowFocused())
					ImGui::FocusWindow(ImGui::GetCurrentWindow());

				if (!ImGui::IsAnyItemActive())
				{
					ImGui::SetActiveID(canvasId, ImGui::GetCurrentWindow());
					const ImGuiIO& io = ImGui::GetIO();
					if (!io.KeyCtrl && !io.KeyShift)
					{
						CurrentCanvasState->NodeGraphState.CurrentGraphInfo.SingleSelectedNodeId = nullptr;   // unselect all
						CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
					}
				}
			}

			if (ImGui::GetActiveID() == canvasId && ImGui::IsMouseDragging(0))
			{
				CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start = ImGui::GetMousePos();
				CurrentCanvasState->NodeGraphState.CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::DragSelecting;
			}
		}
		else if (ImGui::GetActiveID() == canvasId)
			ImGui::ClearActiveID();
		break;
	}
	case SNodeGraphState::CursorState::Dragging:
	{
		if (!ImGui::IsMouseDown(0))
		{
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::None;
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DraggedNodeId = nullptr;
		}
		break;
	}
	case SNodeGraphState::CursorState::DragSelecting:
	{
		if (ImGui::IsMouseDown(0))
		{
			drawList->AddRectFilled(CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start, ImGui::GetMousePos(), CurrentCanvasState->colors[ColSelectBg]);
			drawList->AddRect(CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start, ImGui::GetMousePos(), CurrentCanvasState->colors[ColSelectBorder]);
		}
		else
		{
			ImGui::ClearActiveID();
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::None;
		}
		break;
	}
	}

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::SetWindowFontScale(1.f);
	ImGui::PopID();
}

void WmGui::Nodes(plf::colony<SNode>& aNodes)
{
	for (auto&& node : aNodes)
	{
		WmGui::Node(node);
	}
}

void WmGui::BeginNode(void* aNodeId, ImVec2* aPosition, bool* aSelected)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	//auto* canvas = gCanvas;
	auto& nodeGraphState = CurrentCanvasState->NodeGraphState;
	//
	nodeGraphState.CurrentNodeInfo.Id = aNodeId;
	nodeGraphState.CurrentNodeInfo.Position = aPosition;
	nodeGraphState.CurrentNodeInfo.Selected = aSelected;

	// 0 - curves and bg
	// 1 - node rect
	// 2 - node hover
	// 3 - node content

	// Top-let corner of the node
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + (*aPosition) * CurrentCanvasState->ZoomLevel + CurrentCanvasState->Offset);

	ImGui::PushID(aNodeId);

	ImGui::BeginGroup();    // Slots and content group
	drawList->ChannelsSetCurrent(3);
}

void WmGui::NodeTitle(const char* aTitle)
{
	auto* storage = ImGui::GetStateStorage();
	float nodeWidth = storage->GetFloat(ImGui::GetID("node-width"));

	if (nodeWidth > 0)
	{
		// Center node title
		ImVec2 title_size = ImGui::CalcTextSize(aTitle);
		if (nodeWidth > title_size.x)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + nodeWidth / 2.f - title_size.x / 2.f);
	}

	auto& io = ImGui::GetIO();
	ImGui::PushFont(io.Fonts->Fonts[1]);
	//ImVec2 cursorPos = ImGui::GetCursorPos();

	ImGui::Text(aTitle);

	//ImGui::NewLine();
	ImGui::Dummy({ 0.f, 1.f });
	ImGui::PopFont();
	//ImDrawList* drawList = ImGui::GetWindowDrawList();
	//
	//ImColor nodeColor = CurrentCanvasState->colors[ColNodeActiveBg];
	//const ImGuiStyle& style = ImGui::GetStyle();
	//
	//ImRect nodeRect{
	//ImGui::GetItemRectMin() - style.ItemInnerSpacing * CurrentCanvasState->ZoomLevel,
	//ImGui::GetItemRectMax() + style.ItemInnerSpacing * CurrentCanvasState->ZoomLevel
	//};
	//
	//drawList->AddRectFilled(nodeRect.Min, nodeRect.Max, nodeColor);
}

bool WmGui::IsConnectingCompatibleSlot()
{
	assert(CurrentCanvasState != nullptr);
	auto* canvas = CurrentCanvasState;
	auto& nodeGraphState = canvas->NodeGraphState;

	if (auto* payload = ImGui::GetDragDropPayload())
	{
		auto* dragPayload = (_DragConnectionPayload*)payload->Data;

		// Node can not connect to itself
		if (dragPayload->NodeId == nodeGraphState.CurrentNodeInfo.Id)
			return false;

		if (dragPayload->IsInput == nodeGraphState.CurrentSlotInfo.IsInput)
			return false;

		return true;
	}

	return false;
}

void WmGui::Slot(const bool aIsInput, SSlotInstanceBase& aSlotInstance)
{
	CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title = aSlotInstance.SlotType.Name.c_str();
	CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput = aIsInput;
	CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Color = aSlotInstance.SlotType.GetColor();
	if (aIsInput)
	{
		CurrentCanvasState->NodeGraphState.CurrentSlotInfo.OutputSlotInstance = nullptr;
		CurrentCanvasState->NodeGraphState.CurrentSlotInfo.InputSlotInstance = static_cast<SInputSlotInstanceBase*>(&aSlotInstance);
	}
	else
	{
		CurrentCanvasState->NodeGraphState.CurrentSlotInfo.InputSlotInstance = nullptr;
		CurrentCanvasState->NodeGraphState.CurrentSlotInfo.OutputSlotInstance = static_cast<SOutputSlotInstanceBase*>(&aSlotInstance);
	}

	ImGui::BeginGroup();

	auto* storage = ImGui::GetStateStorage();
	const auto& style = ImGui::GetStyle();
	const float circleRadius = 5.f * CurrentCanvasState->ZoomLevel;

	ImVec2 titleSize = ImGui::CalcTextSize(aSlotInstance.SlotType.Name.c_str());

	float itemOffsetX = 1.f + style.ItemSpacing.x * CurrentCanvasState->ZoomLevel;
	if (aIsInput)
		itemOffsetX = -itemOffsetX;

	ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2{ itemOffsetX, 0 });

	auto* drawList = ImGui::GetWindowDrawList();

	ImGui::BeginGroup();
	if (!aIsInput)
	{
		// Align output slots to the right edge of the node.
		ImGuiID maxWidthId = ImGui::GetID("output-max-title-width");

		// Reset max width if zoom has changed
		ImGuiID canvasZoomLevel = ImGui::GetID("canvas-zoom");
		if (storage->GetFloat(canvasZoomLevel, CurrentCanvasState->ZoomLevel) != CurrentCanvasState->ZoomLevel)
		{
			storage->SetFloat(maxWidthId, 0.0f);
		}
		storage->SetFloat(canvasZoomLevel, CurrentCanvasState->ZoomLevel);

		const f32 outputMaxTitleWidth = ImMax(storage->GetFloat(maxWidthId, titleSize.x), titleSize.x);
		storage->SetFloat(maxWidthId, outputMaxTitleWidth);
		const f32 offset = (outputMaxTitleWidth + style.ItemSpacing.x) - titleSize.x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		ImGui::TextUnformatted(aSlotInstance.SlotType.Name.c_str());
		ImGui::SameLine();
	}

	ImRect circleRect{
	ImGui::GetCursorScreenPos(),
	ImGui::GetCursorScreenPos() + ImVec2{circleRadius * 2, circleRadius * 2}
	};
	// Vertical-align circle in the middle of the line.
	float circleOffsetY = titleSize.y / 2.f - circleRadius;
	circleRect.Min.y += circleOffsetY;
	circleRect.Max.y += circleOffsetY;

	bool shouldDrawHollowConnection = true;

	if (aSlotInstance.Connection)
	{
		shouldDrawHollowConnection = false;
	}
	else if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
	{
		char dataTypeFragment[] = "new-node-connection-";
		if (strncmp(payload->DataType, dataTypeFragment, sizeof(dataTypeFragment) - 1) == 0)
		{
			auto* dragPayload = (_DragConnectionPayload*)payload->Data;

			if (dragPayload->InputSlotInstance == &aSlotInstance || dragPayload->OutputSlotInstance == &aSlotInstance)
			{
				shouldDrawHollowConnection = false;
			}
		}
	}


	if (shouldDrawHollowConnection)
	{
		drawList->ChannelsSetCurrent(2);
		
		ImColor nodeBgColor;

		if (CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Selected && *CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Selected)
			nodeBgColor = CurrentCanvasState->colors[ColNodeActiveBg];
		else
			nodeBgColor = CurrentCanvasState->colors[ColNodeBg];

		drawList->AddCircleFilled(circleRect.GetCenter(), circleRadius - 0.5f, nodeBgColor);
		//drawList->AddCircleFilled(circleRect.GetCenter(), circleRadius, CurrentCanvasState->colors[ColNodeBg]);
		drawList->ChannelsSetCurrent(3);
		drawList->AddCircle(circleRect.GetCenter(), circleRadius - 1.f, aSlotInstance.SlotType.GetColor(), 12, 2.0f);
	}
	else
		drawList->AddCircleFilled(circleRect.GetCenter(), circleRadius, aSlotInstance.SlotType.GetColor());

	ImGui::ItemSize(circleRect.GetSize());
	ImGui::ItemAdd(circleRect, ImGui::GetID(aSlotInstance.SlotType.Name.c_str()));

	ImGui::PushID(&aSlotInstance);

	if (aIsInput)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(aSlotInstance.SlotType.Name.c_str());
		ImGui::SameLine();

		//ImVec2 PrePos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() - ImVec2{ itemOffsetX, 0.f });
		//ImGui::SetCursorScreenPos(PrePos);
	}
	ImGui::EndGroup();


	//if (aIsInput)
	//	ImGui::NewLine();

	ImGui::PopID();

	bool shouldDrawHover = false;
	if (ImGui::IsItemHovered())
		shouldDrawHover = true;

	//const ImGuiStyle& style = ImGui::GetStyle();


	ImGui::PushID(CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title);
	//ImGui::PushID(impl->slot.kind);

	ImRect slotRect{ ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
	ImGui::ItemAdd(slotRect, ImGui::GetID(CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title));

	if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
	{
		ImGui::SetActiveID(ImGui::GetID(aSlotInstance.SlotType.Name.c_str()), ImGui::GetCurrentWindow());
		//*CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Selected = true;
		//if (!ImGui::GetIO().KeyCtrl)
		//{
		//	CurrentCanvasState->NodeGraphState.CurrentGraphInfo.SingleSelectedNodeId = CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id;
		//	CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
		//}
	}

	if (ImGui::IsItemActive() && !ImGui::IsMouseDown(0))
		ImGui::ClearActiveID();

	// Store slot edge positions, curves will connect there
	{
		float x;
		if (aIsInput)
			x = slotRect.Min.x;
		else
			x = slotRect.Max.x;

		CurrentCanvasState->CachedData.SetFloat(MakeSlotDataID("x", CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title, CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id, CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput), circleRect.GetCenter().x);
		CurrentCanvasState->CachedData.SetFloat(MakeSlotDataID("y", CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title, CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id, CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput),
			circleRect.GetCenter().y);
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip))
	{
		auto* payload = ImGui::GetDragDropPayload();
		char dragId[32];
		snprintf(dragId, sizeof(dragId), "new-node-connection-%08X", !CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput);
		if (payload == nullptr || !payload->IsDataType(dragId))
		{
			_DragConnectionPayload dragPayload{ };
			dragPayload.NodeId = CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id;
			dragPayload.IsInput = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput;
			dragPayload.SlotTitle = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title;
			dragPayload.SlotColor = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Color;
			dragPayload.InputSlotInstance = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.InputSlotInstance;
			dragPayload.OutputSlotInstance = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.OutputSlotInstance;

			ImGui::SetDragDropPayload(dragId, &dragPayload, sizeof(dragPayload));

			// Clear new connection info
			CurrentCanvasState->NodeGraphState.NewConnection.InputNodeId	= nullptr;
			CurrentCanvasState->NodeGraphState.NewConnection.InputSlot		= nullptr;
			CurrentCanvasState->NodeGraphState.NewConnection.OutputNodeId	= nullptr;
			CurrentCanvasState->NodeGraphState.NewConnection.OutputSlot = nullptr;
			CurrentCanvasState->NodeGraphState.NewConnection.InputSlotName = nullptr;
			CurrentCanvasState->NodeGraphState.NewConnection.OutputSlotName = nullptr;
		}
		//ImGui::TextUnformatted(CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title);
		ImGui::EndDragDropSource();
	}

	if (IsConnectingCompatibleSlot() && ImGui::BeginDragDropTarget())
	{
		shouldDrawHover = true;

		// Accept drags from opposite type (input <-> output)
		char dragId[32];
		snprintf(dragId, sizeof(dragId), "new-node-connection-%08X", CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput);

		if (auto* payload = ImGui::AcceptDragDropPayload(dragId, ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			auto* dragData = (_DragConnectionPayload*)payload->Data;

			// Store info of source slot to be queried by ImNodes::GetConnection()
			if (!CurrentCanvasState->NodeGraphState.CurrentSlotInfo.IsInput)
			{
				CurrentCanvasState->NodeGraphState.NewConnection.InputNodeId = dragData->NodeId;
				CurrentCanvasState->NodeGraphState.NewConnection.InputSlotName = dragData->SlotTitle;
				CurrentCanvasState->NodeGraphState.NewConnection.InputSlot = dragData->InputSlotInstance;
				CurrentCanvasState->NodeGraphState.NewConnection.OutputNodeId = CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id;
				CurrentCanvasState->NodeGraphState.NewConnection.OutputSlotName = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title;
				CurrentCanvasState->NodeGraphState.NewConnection.OutputSlot = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.OutputSlotInstance;
				CurrentCanvasState->NodeGraphState.NewConnection.Color = dragData->SlotColor;
			}
			else
			{
				CurrentCanvasState->NodeGraphState.NewConnection.InputNodeId = CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id;
				CurrentCanvasState->NodeGraphState.NewConnection.InputSlotName = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title;
				CurrentCanvasState->NodeGraphState.NewConnection.InputSlot = CurrentCanvasState->NodeGraphState.CurrentSlotInfo.InputSlotInstance;
				CurrentCanvasState->NodeGraphState.NewConnection.OutputNodeId = dragData->NodeId;
				CurrentCanvasState->NodeGraphState.NewConnection.OutputSlotName = dragData->SlotTitle;
				CurrentCanvasState->NodeGraphState.NewConnection.Color = dragData->SlotColor;
				CurrentCanvasState->NodeGraphState.NewConnection.OutputSlot = dragData->OutputSlotInstance;
			}
			CurrentCanvasState->NodeGraphState.JustConnected = true;
		}

		ImGui::EndDragDropTarget();
	}

	if (shouldDrawHover)
	{
		drawList->ChannelsSetCurrent(2);
		ImRect rect{ ImGui::GetItemRectMin(),
		ImGui::GetItemRectMax() };


		drawList->AddRectFilled(rect.Min, rect.Max, IM_COL32(255, 255, 255, 100), 2.f);
		drawList->ChannelsSetCurrent(3);
	}

	if (aIsInput)
	{
		ImGui::BeginGroup();
		if (!aSlotInstance.Connection)
			aSlotInstance.Inspect();
		ImGui::EndGroup();
	}


	//ImGui::PopID(); // kind
	ImGui::PopID(); // name

	//[Nicos]: Somehow this makes output nodes appear on the edge instead of completely inside the node
	ImGui::GetCurrentWindow()->DC.CursorMaxPos.x -= itemOffsetX;

	ImGui::EndGroup();
}

void WmGui::InputSlots(std::vector<std::unique_ptr<SInputSlotInstanceBase>>& slots)
{
	const auto& style = ImGui::GetStyle();

	// Render input slots
	ImGui::BeginGroup();
	{
		for (auto&& slot : slots)
		{
			SInputSlotInstanceBase& instance = *slot;
			Slot(true, instance);
		}
	}
	ImGui::EndGroup();

	// Move cursor to the next column
	ImGui::SetCursorScreenPos({ ImGui::GetItemRectMax().x + style.ItemSpacing.x, ImGui::GetItemRectMin().y });

	// Begin region for node content
	ImGui::BeginGroup();
}

void WmGui::OutputSlots(std::vector<std::unique_ptr<SOutputSlotInstanceBase>>& slots)
{
	const auto& style = ImGui::GetStyle();

	// End region of node content
	ImGui::EndGroup();

	// Render output slots in the next column
	ImGui::SetCursorScreenPos({ ImGui::GetItemRectMax().x + style.ItemSpacing.x, ImGui::GetItemRectMin().y });
	ImGui::BeginGroup();
	{
		for (auto&& slot : slots)
			Slot(false, *slot);
	}
	ImGui::EndGroup();
}

void WmGui::EndNode()
{
	//assert(gCanvas != nullptr);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 5.f, 5.f });
	const ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	//auto* canvas = gCanvas;
	auto& nodeGraphState = CurrentCanvasState->NodeGraphState;
	auto* nodeId = nodeGraphState.CurrentNodeInfo.Id;
	//
	bool& isNodeSelected = *nodeGraphState.CurrentNodeInfo.Selected;
	ImVec2& nodePosition = *nodeGraphState.CurrentNodeInfo.Position;
	//

	ImGui::EndGroup();    // Slots and content group

	ImRect nodeRect{
		ImGui::GetItemRectMin() - style.ItemInnerSpacing * CurrentCanvasState->ZoomLevel,
		ImGui::GetItemRectMax() + style.ItemInnerSpacing * CurrentCanvasState->ZoomLevel
	};

	// Render frame
	drawList->ChannelsSetCurrent(1);
	
	ImColor nodeColor = isNodeSelected ? CurrentCanvasState->colors[ColNodeActiveBg] : CurrentCanvasState->colors[ColNodeBg];
	drawList->AddRectFilled(nodeRect.Min, nodeRect.Max, nodeColor, 5.0f);
	drawList->AddRect(nodeRect.Min, nodeRect.Max, CurrentCanvasState->colors[ColNodeBorder], 5.0f, 15, 2.f);

	// Create node item
	ImGuiID nodeItemId = ImGui::GetID(nodeId);
	ImGui::ItemSize(nodeRect.GetSize());
	ImGui::ItemAdd(nodeRect, nodeItemId);

	// Node is active when being dragged
	if (ImGui::IsMouseDown(0) && !ImGui::IsAnyItemActive() && ImGui::IsItemHovered())
		ImGui::SetActiveID(nodeItemId, ImGui::GetCurrentWindow());
	else if (!ImGui::IsMouseDown(0) && ImGui::IsItemActive())
		ImGui::ClearActiveID();

	// Save last selection state in case we are about to start dragging multiple selected nodes
	if (ImGui::IsMouseClicked(0))
	{
		ImGuiID previouslySelectedId = ImHashStr("prev-selected", 0, ImHashData(&CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id, sizeof(CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id)));
		CurrentCanvasState->CachedData.SetBool(previouslySelectedId, isNodeSelected);
	}

	ImGuiIO& io = ImGui::GetIO();
	switch (CurrentCanvasState->NodeGraphState.CurrentGraphInfo.CurrentCursorState)
	{
	case SNodeGraphState::CursorState::None:
	{
		isNodeSelected = NodeSelectionBehavior(isNodeSelected);
		break;
	}
	case SNodeGraphState::CursorState::Dragging:
	{
		nodePosition = NodeDraggingBehavior(isNodeSelected, nodePosition);
		break;
	}
	case SNodeGraphState::CursorState::DragSelecting:
	{
		isNodeSelected = DragSelectionBehavior(nodeRect, isNodeSelected);
		break;
	}
	}

	auto* storage = ImGui::GetStateStorage();
	storage->SetFloat(ImGui::GetID("node-width"), ImGui::GetItemRectSize().x);

	ImGui::PopStyleVar();
	ImGui::PopID();
}

void WmGui::Connections(plf::colony<SConnection>& aConnections)
{
	auto it = aConnections.begin();

	while (it != aConnections.end())
	{
		auto&& connection = *it;
		auto& style = ImGui::GetStyle();
		if (!WmGui::Connection(connection.InputNodeId, connection.InputSlotName, connection.OutputNodeId, connection.OutputSlotName, connection.Color))
		{
			it->InputSlot->Connection = nullptr;
			it->OutputSlot->Connection = nullptr;
			it = aConnections.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void WmGui::PotentialConnection(NodeGraph& aNodeGraph)
{
	SConnection potentialConnection;

	if (WmGui::GetNewConnection(&potentialConnection.InputNodeId, &potentialConnection.InputSlotName, &potentialConnection.OutputNodeId, &potentialConnection.OutputSlotName, &potentialConnection.Color, &potentialConnection.InputSlot, &potentialConnection.OutputSlot))
	{
		for (auto it = aNodeGraph.Connections.begin(); it != aNodeGraph.Connections.end(); it++)
		{
			if (it->InputNodeId == potentialConnection.InputNodeId && strcmp(it->InputSlotName, potentialConnection.InputSlotName) == 0)
			{
				aNodeGraph.Connections.erase(it);
				break;
			}
		}

		auto insertedConnectionIt = aNodeGraph.Connections.insert(potentialConnection);

		insertedConnectionIt->InputSlot->Connection = &*insertedConnectionIt;
		insertedConnectionIt->OutputSlot->Connection = &*insertedConnectionIt;
	}
}

void WmGui::DrawPendingConnection()
{
	if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
	{
		char dataTypeFragment[] = "new-node-connection-";
		if (strncmp(payload->DataType, dataTypeFragment, sizeof(dataTypeFragment) - 1) == 0)
		{
			auto* dragPayload = (_DragConnectionPayload*)payload->Data;

			ImVec2 slotPosition{
				CurrentCanvasState->CachedData.GetFloat(MakeSlotDataID("x", dragPayload->SlotTitle, dragPayload->NodeId,
					dragPayload->IsInput)),
				CurrentCanvasState->CachedData.GetFloat(MakeSlotDataID("y", dragPayload->SlotTitle, dragPayload->NodeId,
					dragPayload->IsInput)),
			};

			const float connectionIndent = 5.f * CurrentCanvasState->ZoomLevel;

			ImVec2 inputPosition, outputPosition;
			if (dragPayload->IsInput)
			{
				inputPosition = slotPosition;
				//inputPosition.x += connectionIndent;
				outputPosition = ImGui::GetMousePos();
			}
			else
			{
				inputPosition = ImGui::GetMousePos();
				outputPosition = slotPosition;
				//outputPosition.x -= connectionIndent;
			}

			auto& style = ImGui::GetStyle();
			RenderConnection(inputPosition, outputPosition, 2.f, dragPayload->SlotColor);
		}
	}
}

bool WmGui::NodeSelectionBehavior(const bool aWasSelected)
{
	auto& io = ImGui::GetIO();
	bool isNodeSelected = aWasSelected;
	auto* nodeId = CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id;
	// Node selection behavior. Selection can change only when no node is being dragged and connections are not being made.
	if (CurrentCanvasState->NodeGraphState.JustConnected || ImGui::GetDragDropPayload() != nullptr)
	{
		// No selections are performed when nodes are being connected.
		CurrentCanvasState->NodeGraphState.JustConnected = false;
	}
	else if (CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DoSelectionsFrame == ImGui::GetCurrentContext()->FrameCount)
	{
		// Unselect other nodes when some node was left-clicked.
		isNodeSelected = CurrentCanvasState->NodeGraphState.CurrentGraphInfo.SingleSelectedNodeId == nodeId;
	}
	else if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && ImGui::IsItemActive())
	{
		isNodeSelected = true;
		if (!io.KeyCtrl && isNodeSelected)
		{
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.SingleSelectedNodeId = nodeId;
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
		}
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		CurrentCanvasState->NodeGraphState.CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::Dragging;
		if (CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DraggedNodeId == nullptr)
		{
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DraggedNodeId = nodeId;
			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.ShouldDragAllSelectedNodes = isNodeSelected;
		}
		else
		{

			CurrentCanvasState->NodeGraphState.CurrentGraphInfo.SingleSelectedNodeId = nullptr;
		}	
	}

	return isNodeSelected;
}

ImVec2 WmGui::NodeDraggingBehavior(const bool isNodeSelected, const ImVec2 aInitialNodePosition)
{
	ImVec2 newNodePosition = aInitialNodePosition;
	if (ImGui::IsMouseDown(0))
	{
		// Node dragging behavior. Drag node under mouse and other selected nodes if current node is selected.
		if ((ImGui::IsItemActive() || (CurrentCanvasState->NodeGraphState.CurrentGraphInfo.DraggedNodeId && CurrentCanvasState->NodeGraphState.CurrentGraphInfo.ShouldDragAllSelectedNodes && isNodeSelected)))
			newNodePosition += ImGui::GetIO().MouseDelta / CurrentCanvasState->ZoomLevel;
	}	

	return newNodePosition;
}

bool WmGui::DragSelectionBehavior(ImRect nodeRect, const bool aNodeWasInitiallySelected)
{
	bool isSelected = aNodeWasInitiallySelected;
	ImRect selectionArea;
	selectionArea.Min.x = ImMin(CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start.x, ImGui::GetMousePos().x);
	selectionArea.Min.y = ImMin(CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start.y, ImGui::GetMousePos().y);
	selectionArea.Max.x = ImMax(CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start.x, ImGui::GetMousePos().x);
	selectionArea.Max.y = ImMax(CurrentCanvasState->NodeGraphState.CurrentGraphInfo.selection_start.y, ImGui::GetMousePos().y);

	ImGuiID prevoiusSelectedId = ImHashStr("prev-selected", 0, ImHashData(&CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id, sizeof(CurrentCanvasState->NodeGraphState.CurrentNodeInfo.Id)));
	if (ImGui::GetIO().KeyShift)
	{
		// Append selection
		if (selectionArea.Overlaps(nodeRect))
			isSelected = true;
		else
			isSelected = CurrentCanvasState->CachedData.GetBool(prevoiusSelectedId);
	}
	else if (ImGui::GetIO().KeyCtrl)
	{
		// Subtract from selection
		if (selectionArea.Overlaps(nodeRect))
			isSelected = false;
		else
			isSelected = CurrentCanvasState->CachedData.GetBool(prevoiusSelectedId);
	}
	else
	{
		// Assign selection
		isSelected = selectionArea.Overlaps(nodeRect);
	}

	return isSelected;
}

void WmGui::ContextMenu(NodeGraph& aNodeGraph)
{
	bool shouldOpenContextMenu = false;

	const ImVec2 canvasPos = ImGui::GetCurrentWindow()->Pos;

	// Open context menu
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) || !ImGui::IsAnyItemHovered()))
		shouldOpenContextMenu = true;

	if (shouldOpenContextMenu)
		ImGui::OpenPopup("context_menu");

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if (ImGui::BeginPopup("context_menu"))
	{
		for(auto& availableNode : aNodeGraph.RegisteredNodes)
		{
			if (ImGui::MenuItem(availableNode.Name.c_str(), nullptr, false)) { availableNode.AddNodeLambda(ImGui::GetMousePos() - canvasPos); }
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

WmGui::SCanvasState::SCanvasState()
{
	auto& imguiStyle = ImGui::GetStyle();
	colors[ColNodeBg] = IM_COL32(20, 20, 22, 255);
	colors[ColNodeActiveBg] = IM_COL32(85, 85, 110, 255);
	colors[ColNodeBorder] = IM_COL32(115, 115, 130, 255);
	colors[ColSelectBg] = imguiStyle.Colors[ImGuiCol_FrameBgActive];
	colors[ColSelectBg].Value.w = 0.25f;
	colors[ColSelectBorder] = imguiStyle.Colors[ImGuiCol_Border];
}
