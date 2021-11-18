#include "pch.h"
#include "NodeGraphGui.h"
#include "NodeGraph/NodeGraph.h"
#include "NodeGraph/NodeTypes.h"

//stack so that we can nest node graphs
static std::vector<WmGui::NodeGraphEditor::SNodeGraphState*> NodeGraphStack;
static WmGui::NodeGraphEditor::SNodeGraphState* CurrentNodeGraph;

/// Struct containing information about connection source node and slot.
struct _DragConnectionPayload
{
	SNode* NodePointer = nullptr;
	
	bool IsInput = 0;

	SInputSlotInstanceBase* InputSlotInstance;
	SOutputSlotInstanceBase* OutputSlotInstance;

	ImColor SlotColor;
};

void WmGui::NodeGraphEditor::NodeGraphEdit(NodeGraph& aNodeGraph, const bool aShowMenu)
{
	if (!aNodeGraph.ShouldBeVisible)
		return;

	//[Nicos]: inline unique id into the name as push id doesn't work with begin -- otherwise we would not be able to stack several node graphs with the same name.
	const std::string name = aNodeGraph.GetName();
	if (ImGui::Begin((std::string(name) + "###" + std::string(aNodeGraph.GetIdentifier())).c_str(), &aNodeGraph.ShouldBeVisible, ImGuiWindowFlags_MenuBar))
	{
		if (aShowMenu)
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::Button("Compile"))
					aNodeGraph.CompileExternal();

				if (ImGui::Button("Save"))
					aNodeGraph.Save();

				if (ImGui::Button("Execute"))
					aNodeGraph.ExecuteExternal();

				ImGui::EndMenuBar();
			}
		}

		CurrentNodeGraph = &aNodeGraph.GetNodeGraphGuiState();
		CurrentNodeGraph->Id = aNodeGraph.GetUniqueId();
		NodeGraphStack.push_back(CurrentNodeGraph);

		WmGui::BeginCanvas(&CurrentNodeGraph->CanvasState);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 55, 255));
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->ChannelsSplit(4);

		WmGui::NodeGraphEditor::Nodes(aNodeGraph);

		drawList->ChannelsSetCurrent(0);

		WmGui::NodeGraphEditor::Connections(aNodeGraph.GetConnections(), aNodeGraph);

		WmGui::NodeGraphEditor::PotentialConnection(aNodeGraph);

		WmGui::NodeGraphEditor::DrawPendingConnection();

		drawList->ChannelsSetCurrent(3);
		WmGui::NodeGraphEditor::ContextMenu(aNodeGraph);

		drawList->ChannelsMerge();

		WmGui::NodeGraphEditor::UpdateNodeGraphCursor();

		ImGui::PopStyleColor();
		WmGui::EndCanvas();

		NodeGraphStack.erase(NodeGraphStack.end() - 1);
		if (NodeGraphStack.size() > 0)
			CurrentNodeGraph = NodeGraphStack[NodeGraphStack.size() - 1];

		CurrentNodeGraph->CurrentGraphInfo.WasMiddleMouseDragging = ImGui::IsMouseDragging(2);
	}


	ImGui::End();
}

ImU32 WmGui::NodeGraphEditor::MakeSlotDataID(const char* aData, const char* aSlotTitle, SNode* aNodePointer, bool aIsInput)
{
	assert(aData != nullptr);
	assert(aSlotTitle != nullptr);
	assert(aNodePointer != nullptr);

	ImU32 SlotId = ImHashStr(aSlotTitle, 0, ImHashData(&aNodePointer, sizeof(aNodePointer)));
	if (aIsInput)
	{
		// Ensure that input and output slots with same name have different ids.
		SlotId ^= ~0U;
	}
	return ImHashStr(aData, 0, SlotId);
}
 
bool WmGui::NodeGraphEditor::RenderConnection(const ImVec2& aInputPosition, const ImVec2& aOutputPosition, f32 aThiccness, const ImColor aColor)
{
	//[Nicos]: WARNING: This code contains a bunch of nonsensical maths to make connection corners look better. I don't know how it works, it just does(kinda)

	const ImVec2 outdentOffset = { 30.f, 0.f };

	const ImVec2 inputOutdent = aInputPosition - outdentOffset;
	const ImVec2 outputOutdent = aOutputPosition + outdentOffset;

	const ImVec2 direction = outputOutdent - inputOutdent;

	SVector2f directionNormalized = { direction.x, direction.y };
	directionNormalized.Normalize();
	const f32 dot = fabsf(directionNormalized.Dot({ 0.f, 1.f }));
	directionNormalized *= dot;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	aThiccness *= CurrentNodeGraph->CanvasState.ZoomLevel.x;

	auto isCloseLambda = [](ImVec2 aStart, ImVec2 aEnd) {
		const ImVec2 closestPoint = ImLineClosestPoint(aStart, aEnd, ImGui::GetMousePos());
		const f32 minimumSquareDistance = ImFabs(ImLengthSqr(ImGui::GetMousePos() - closestPoint));
		const bool isClose = minimumSquareDistance <= 200;

		return isClose;
	};

	bool isClose = isCloseLambda(inputOutdent, outputOutdent) || isCloseLambda(aInputPosition, inputOutdent) || isCloseLambda(outputOutdent, aOutputPosition);

	if (!isClose)
		directionNormalized *= 0.5f;

	const ImVec2 imDirectionNormalized = { directionNormalized.X, directionNormalized.Y };
	aThiccness = isClose ? aThiccness * 2.f : aThiccness;

	drawList->AddLine(inputOutdent - (imDirectionNormalized * (aThiccness) * 0.5f) + ImVec2{ dot * aThiccness , 0.f }, outputOutdent + (imDirectionNormalized * (aThiccness) * 0.5f) - ImVec2{ dot * aThiccness + 0.2f, 0.f }, aColor, (aThiccness));
	drawList->AddLine(aInputPosition, inputOutdent + ImVec2{ dot * aThiccness * 1.f, 0.f }, aColor, aThiccness);
	drawList->AddLine(aOutputPosition, outputOutdent - ImVec2{ dot * aThiccness * 1.f, 0.f }, aColor, aThiccness);

	return isClose;
}

bool WmGui::NodeGraphEditor::GetNewConnection(SNode** aInputNodePointer, SNode** aOutputNodePointer, ImColor* aColor, SInputSlotInstanceBase** aInputSlotInstance, SOutputSlotInstanceBase** aOutputSlotInstance)
{
	assert(CurrentNodeGraph != nullptr);
	assert(aInputNodePointer != nullptr);
	assert(aOutputNodePointer != nullptr);
	assert(aColor != nullptr);

	auto& nodeGraphState = *CurrentNodeGraph;

	if (nodeGraphState.NewConnection.OutputNodePointer != nullptr)
	{
		*aInputNodePointer = nodeGraphState.NewConnection.InputNodePointer;
		*aOutputNodePointer = nodeGraphState.NewConnection.OutputNodePointer;
		*aInputSlotInstance = nodeGraphState.NewConnection.InputSlot;
		*aOutputSlotInstance = nodeGraphState.NewConnection.OutputSlot;
		*aColor = nodeGraphState.NewConnection.Color;

		memset(&nodeGraphState.NewConnection, 0, sizeof(nodeGraphState.NewConnection));
		return true;
	}

	return false;
}

bool WmGui::NodeGraphEditor::Connection(SNode* aInputNode, SInputSlotInstanceBase* aInputSlot, SNode* aOutputNode, SOutputSlotInstanceBase* aOutputSlot, ImColor color)
{
	assert(CurrentNodeGraph != nullptr);
	assert(aInputNode != nullptr);
	assert(aOutputNode != nullptr);

	bool isConnected = true;

	const ImVec2 input_slot_pos{
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("x", aInputSlot->SlotType.Name.c_str(), aInputNode, true)),
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("y", aInputSlot->SlotType.Name.c_str(), aInputNode, true)),
	};

	const ImVec2 output_slot_pos{
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("x", aOutputSlot->SlotType.Name.c_str(), aOutputNode, false)),
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("y", aOutputSlot->SlotType.Name.c_str(), aOutputNode, false)),
	};

	const bool isCurveHovered = RenderConnection(input_slot_pos, output_slot_pos, 2.0f, color);
	if (isCurveHovered && ImGui::IsWindowHovered())
	{
		ImGuiIO& io = ImGui::GetIO();

		if ((ImGui::IsMouseReleased(0) && io.KeyAlt) || ImGui::IsMouseReleased(2) && !CurrentNodeGraph->CurrentGraphInfo.WasMiddleMouseDragging)
			isConnected = false;
	}

	CurrentNodeGraph->CachedData.SetBool(MakeSlotDataID("hovered", aInputSlot->SlotType.Name.c_str(), aInputNode, true), isCurveHovered && isConnected);
	CurrentNodeGraph->CachedData.SetBool(MakeSlotDataID("hovered", aOutputSlot->SlotType.Name.c_str(), aOutputNode, false), isCurveHovered && isConnected);

	return isConnected;
}

void WmGui::NodeGraphEditor::Node(SNode& aNode)
{
	WmGui::NodeGraphEditor::BeginNode(&aNode, &aNode.Position, &aNode.Selected);
	WmGui::NodeGraphEditor::NodeTitle(aNode.NodeType.GetTitle(aNode).c_str());

	const ImU32 col = aNode.Selected ? CurrentNodeGraph->Colors[static_cast<i32>(StyleColor::ColSlotEditActiveBg)] : CurrentNodeGraph->Colors[static_cast<i32>(StyleColor::ColSlotEditBg)];
	ImGui::PushStyleColor(ImGuiCol_FrameBg, col);

	WmGui::NodeGraphEditor::InputSlots(aNode.InputSlotInstances, *aNode.NodeGraph);

	//[Nicos]: TODO add a way to preview stuff here - depending on compiler and node

	WmGui::NodeGraphEditor::OutputSlots(aNode.OutputSlotInstances, *aNode.NodeGraph);

	ImGui::PopStyleColor();

	WmGui::NodeGraphEditor::EndNode();

	ImGuiIO& io = ImGui::GetIO();

	constexpr i32 deleteKey = 261; //[Nicos]: TODO Proper inputs for imgui | ImGuiKey_Delete is incorrect here, but not sure why.

	if (aNode.Selected && io.KeysDown[deleteKey])
		aNode.IWantToDie = true;
}

void WmGui::NodeGraphEditor::Nodes(NodeGraph& aNodeGraph)
{
	for (auto&& node : aNodeGraph.GetNodes())
		WmGui::NodeGraphEditor::Node(node);

	aNodeGraph.KillNodesThatWantToDie();
}


void WmGui::NodeGraphEditor::BeginNode(SNode* aNodePointer, ImVec2* aPosition, bool* aSelected)
{
	assert(aNodePointer != nullptr);
	assert(aPosition != nullptr);
	assert(aSelected != nullptr);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto& nodeGraphState = *CurrentNodeGraph;
	nodeGraphState.CurrentNodeInfo.NodePointer = aNodePointer;
	nodeGraphState.CurrentNodeInfo.Position = aPosition;
	nodeGraphState.CurrentNodeInfo.Selected = aSelected;

	// Top-let corner of the node
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + (*aPosition) * CurrentNodeGraph->CanvasState.ZoomLevel + CurrentNodeGraph->CanvasState.Offset);

	ImGui::PushID(aNodePointer);
	IdCount++;

	ImGui::BeginGroup();    // Slots and content group
	drawList->ChannelsSetCurrent(3);
}

void WmGui::NodeGraphEditor::NodeTitle(const char* aTitle)
{
	auto* storage = ImGui::GetStateStorage();
	f32 nodeWidth = storage->GetFloat(ImGui::GetID("node-width"));

	if (nodeWidth > 0)
	{
		// Center node title
		ImVec2 title_size = ImGui::CalcTextSize(aTitle);
		if (nodeWidth > title_size.x)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + nodeWidth / 2.f - title_size.x / 2.f);
	}

	auto& io = ImGui::GetIO();
	ImGui::PushFont(io.Fonts->Fonts[1]);

	ImGui::Text(aTitle);

	ImGui::Dummy({ 0.f, 1.f });
	ImGui::PopFont();
}

bool WmGui::NodeGraphEditor::IsConnectingCompatibleSlot()
{
	assert(CurrentNodeGraph != nullptr);
	auto& nodeGraphState = *CurrentNodeGraph;

	if (auto* payload = ImGui::GetDragDropPayload())
	{
		std::string connection = WmGui::NodeGraphEditor::GetNewNodeConnectionString();
		if (strncmp(payload->DataType, connection.c_str(), connection.size()) == 0)
		{
			auto* dragPayload = (_DragConnectionPayload*)payload->Data;

			// Node can not connect to itself
			if (dragPayload->NodePointer == nodeGraphState.CurrentNodeInfo.NodePointer)
				return false;

			if (dragPayload->IsInput == nodeGraphState.CurrentSlotInfo.IsInput)
				return false;

			if (dragPayload->IsInput && !dragPayload->InputSlotInstance->SlotType.IsCompatibleWith(nodeGraphState.CurrentSlotInfo.OutputSlotInstance->SlotType))
				return false;

			if (!dragPayload->IsInput && !dragPayload->OutputSlotInstance->SlotType.IsCompatibleWith(nodeGraphState.CurrentSlotInfo.InputSlotInstance->SlotType))
				return false;

			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

void WmGui::NodeGraphEditor::Slot(const bool aIsInput, SSlotInstanceBase& aSlotInstance, NodeGraph& aNodeGraph)
{
	CurrentNodeGraph->CurrentSlotInfo.Title = aSlotInstance.SlotType.Name;
	CurrentNodeGraph->CurrentSlotInfo.IsInput = aIsInput;
	CurrentNodeGraph->CurrentSlotInfo.Color = aSlotInstance.SlotType.GetColor();
	if (aIsInput)
	{
		CurrentNodeGraph->CurrentSlotInfo.OutputSlotInstance = nullptr;
		CurrentNodeGraph->CurrentSlotInfo.InputSlotInstance = static_cast<SInputSlotInstanceBase*>(&aSlotInstance);
	}
	else
	{
		CurrentNodeGraph->CurrentSlotInfo.InputSlotInstance = nullptr;
		CurrentNodeGraph->CurrentSlotInfo.OutputSlotInstance = static_cast<SOutputSlotInstanceBase*>(&aSlotInstance);
	}

	ImGui::BeginGroup();

	auto* storage = ImGui::GetStateStorage();
	const auto& style = ImGui::GetStyle();
	const f32 circleRadius = 6.5f * CurrentNodeGraph->CanvasState.ZoomLevel.x;

	ImVec2 titleSize = ImGui::CalcTextSize(aSlotInstance.SlotType.Name.c_str());

	f32 itemOffsetX = 2.5f + style.ItemSpacing.x * CurrentNodeGraph->CanvasState.ZoomLevel.x;
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
		if (storage->GetFloat(canvasZoomLevel, CurrentNodeGraph->CanvasState.ZoomLevel.x) != CurrentNodeGraph->CanvasState.ZoomLevel.x)
		{
			storage->SetFloat(maxWidthId, 0.0f);
		}
		storage->SetFloat(canvasZoomLevel, CurrentNodeGraph->CanvasState.ZoomLevel.x);

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
	f32 circleOffsetY = titleSize.y / 2.f - circleRadius;
	circleRect.Min.y += circleOffsetY;
	circleRect.Max.y += circleOffsetY;

	bool shouldDrawHollowConnection = true;

	if (aSlotInstance.HasConnection())
	{
		shouldDrawHollowConnection = false;
	}
	else if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
	{
		std::string connection = WmGui::NodeGraphEditor::GetNewNodeConnectionString();
		if (strncmp(payload->DataType, connection.c_str(), connection.size()) == 0)
		{
			auto* dragPayload = (_DragConnectionPayload*)payload->Data;

			if (dragPayload->InputSlotInstance == &aSlotInstance || dragPayload->OutputSlotInstance == &aSlotInstance)
				shouldDrawHollowConnection = false;
		}
	}


	if (shouldDrawHollowConnection)
	{
		drawList->ChannelsSetCurrent(2);

		ImColor nodeBgColor;

		if (CurrentNodeGraph->CurrentNodeInfo.Selected && *(CurrentNodeGraph->CurrentNodeInfo.Selected))
			nodeBgColor = CurrentNodeGraph->Colors[static_cast<i32>(StyleColor::ColNodeActiveBg)];
		else
			nodeBgColor = CurrentNodeGraph->Colors[static_cast<i32>(StyleColor::ColNodeBg)];

		drawList->AddCircleFilled(circleRect.GetCenter(), circleRadius - 0.5f, nodeBgColor);
		drawList->ChannelsSetCurrent(3);
		drawList->AddCircle(circleRect.GetCenter(), circleRadius - 1.f, aSlotInstance.SlotType.GetColor(), 12, 2.0f);
	}
	else
		drawList->AddCircleFilled(circleRect.GetCenter(), circleRadius, aSlotInstance.SlotType.GetColor());

	ImGui::ItemSize(circleRect.GetSize());
	ImGui::ItemAdd(circleRect, ImGui::GetID(aSlotInstance.SlotType.Name.c_str()));

	ImGui::PushID(&aSlotInstance);
	IdCount++;

	if (aIsInput)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(aSlotInstance.SlotType.Name.c_str());
		ImGui::SameLine();

		ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() - ImVec2{ itemOffsetX, 0.f });
	}
	ImGui::EndGroup();

	ImGui::PopID();
	IdCount--;

	bool shouldDrawHover = false;
	if (ImGui::IsItemHovered())
		shouldDrawHover = true;

	//const ImGuiStyle& style = ImGui::GetStyle();

	ImGui::PushID(CurrentNodeGraph->CurrentSlotInfo.Title.c_str());
	IdCount++;

	ImRect slotRect{ ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
	ImGui::ItemAdd(slotRect, ImGui::GetID(CurrentNodeGraph->CurrentSlotInfo.Title.c_str()));

	if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
		ImGui::SetActiveID(ImGui::GetID(aSlotInstance.SlotType.Name.c_str()), ImGui::GetCurrentWindow());

	if (ImGui::IsItemActive() && !ImGui::IsMouseDown(0))
		ImGui::ClearActiveID();

	// Store slot edge positions, curves will connect there
	{
		f32 x;
		if (aIsInput)
			x = slotRect.Min.x;
		else
			x = slotRect.Max.x;

		CurrentNodeGraph->CachedData.SetFloat(MakeSlotDataID("x", CurrentNodeGraph->CurrentSlotInfo.Title.c_str(), CurrentNodeGraph->CurrentNodeInfo.NodePointer, CurrentNodeGraph->CurrentSlotInfo.IsInput), circleRect.GetCenter().x);
		CurrentNodeGraph->CachedData.SetFloat(MakeSlotDataID("y", CurrentNodeGraph->CurrentSlotInfo.Title.c_str(), CurrentNodeGraph->CurrentNodeInfo.NodePointer, CurrentNodeGraph->CurrentSlotInfo.IsInput),
			circleRect.GetCenter().y);
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip))
	{
		auto* payload = ImGui::GetDragDropPayload();
		char dragId[32];

		std::string connectionStr = WmGui::NodeGraphEditor::GetNewNodeConnectionString();
		connectionStr += !CurrentNodeGraph->CurrentSlotInfo.IsInput;
		if (payload == nullptr || !payload->IsDataType(dragId))
		{
			_DragConnectionPayload dragPayload{ };
			dragPayload.NodePointer = CurrentNodeGraph->CurrentNodeInfo.NodePointer;
			dragPayload.IsInput = CurrentNodeGraph->CurrentSlotInfo.IsInput;

			dragPayload.SlotColor = CurrentNodeGraph->CurrentSlotInfo.Color;
			dragPayload.InputSlotInstance = CurrentNodeGraph->CurrentSlotInfo.InputSlotInstance;
			dragPayload.OutputSlotInstance = CurrentNodeGraph->CurrentSlotInfo.OutputSlotInstance;

			ImGui::SetDragDropPayload(connectionStr.c_str(), &dragPayload, sizeof(dragPayload));

			// Clear new connection info
			CurrentNodeGraph->NewConnection.InputNodePointer = nullptr;
			CurrentNodeGraph->NewConnection.InputSlot = nullptr;
			CurrentNodeGraph->NewConnection.OutputNodePointer = nullptr;
			CurrentNodeGraph->NewConnection.OutputSlot = nullptr;
		}
		//ImGui::TextUnformatted(CurrentCanvasState->NodeGraphState.CurrentSlotInfo.Title);
		ImGui::EndDragDropSource();
	}

	std::string connectionStr = WmGui::NodeGraphEditor::GetNewNodeConnectionString();

	// Accept drags from opposite type (input <-> output)
	connectionStr += CurrentNodeGraph->CurrentSlotInfo.IsInput;

	auto* payload = ImGui::GetDragDropPayload();
	if (IsConnectingCompatibleSlot() && ImGui::BeginDragDropTarget() && payload->IsDataType(connectionStr.c_str()))
	{
		shouldDrawHover = true;
		payload = ImGui::AcceptDragDropPayload(connectionStr.c_str(), ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

		if (payload)
		{
			auto* dragData = (_DragConnectionPayload*)payload->Data;

			CurrentNodeGraph->NewConnection = SConnection();
			// Store info of source slot to be queried by ImNodes::GetConnection()
			if (!CurrentNodeGraph->CurrentSlotInfo.IsInput)
			{
				CurrentNodeGraph->NewConnection.InputNodePointer = dragData->NodePointer;
				CurrentNodeGraph->NewConnection.InputSlot = dragData->InputSlotInstance;
				CurrentNodeGraph->NewConnection.OutputNodePointer = CurrentNodeGraph->CurrentNodeInfo.NodePointer;
				CurrentNodeGraph->NewConnection.OutputSlot = CurrentNodeGraph->CurrentSlotInfo.OutputSlotInstance;
				CurrentNodeGraph->NewConnection.Color = dragData->SlotColor;
			}
			else
			{
				CurrentNodeGraph->NewConnection.InputNodePointer = CurrentNodeGraph->CurrentNodeInfo.NodePointer;
				CurrentNodeGraph->NewConnection.InputSlot = CurrentNodeGraph->CurrentSlotInfo.InputSlotInstance;
				CurrentNodeGraph->NewConnection.OutputNodePointer = dragData->NodePointer;
				CurrentNodeGraph->NewConnection.OutputSlot = dragData->OutputSlotInstance;
				CurrentNodeGraph->NewConnection.Color = dragData->SlotColor;
			}
			CurrentNodeGraph->JustConnected = true;
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
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		auto before = window->IDStack.Size;
		ImGui::BeginGroup();
		if (!aSlotInstance.HasConnection())
		{
			const EAlterStatus modified = aSlotInstance.Inspect(aNodeGraph);

			if (modified == EAlterStatus::Changed)
				aNodeGraph.MarkDirty();
		}
		ImGui::EndGroup();
		auto after = window->IDStack.Size;

		assert(before == after);
	}

	ImGui::PopID(); // name
	IdCount--;
	//[Nicos]: Somehow this makes output nodes appear on the edge instead of completely inside the node
	ImGui::GetCurrentWindow()->DC.CursorMaxPos.x -= itemOffsetX;

	ImGui::EndGroup();
}

void WmGui::NodeGraphEditor::InputSlots(std::vector<std::unique_ptr<SInputSlotInstanceBase>>& aSlots, NodeGraph& aNodeGraph)
{
	const auto& style = ImGui::GetStyle();

	// Render input slots
	ImGui::BeginGroup();
	{
		for (auto&& slot : aSlots)
		{
			SInputSlotInstanceBase& instance = *slot;
			Slot(true, instance, aNodeGraph);
		}
	}
	ImGui::EndGroup();

	// Move cursor to the next column
	ImGui::SetCursorScreenPos({ ImGui::GetItemRectMax().x + style.ItemSpacing.x, ImGui::GetItemRectMin().y });

	// Begin region for node content
	ImGui::BeginGroup();
}

void WmGui::NodeGraphEditor::OutputSlots(std::vector<std::unique_ptr<SOutputSlotInstanceBase>>& slots, NodeGraph& aNodeGraph)
{
	const auto& style = ImGui::GetStyle();

	// End region of node content
	ImGui::EndGroup();

	// Render output slots in the next column
	ImGui::SetCursorScreenPos({ ImGui::GetItemRectMax().x + style.ItemSpacing.x, ImGui::GetItemRectMin().y });
	ImGui::BeginGroup();
	{
		for (auto&& slot : slots)
			Slot(false, *slot, aNodeGraph);
	}
	ImGui::EndGroup();
}

void WmGui::NodeGraphEditor::EndNode()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 5.f, 5.f });
	const ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	auto& nodeGraphState = *CurrentNodeGraph;
	auto* nodePointer = nodeGraphState.CurrentNodeInfo.NodePointer;

	bool& isNodeSelected = *nodeGraphState.CurrentNodeInfo.Selected;
	ImVec2& nodePosition = *nodeGraphState.CurrentNodeInfo.Position;

	ImGui::EndGroup();    // Slots and content group

	ImRect nodeRect{
		ImGui::GetItemRectMin() - style.ItemInnerSpacing * nodeGraphState.CanvasState.ZoomLevel,
		ImGui::GetItemRectMax() + style.ItemInnerSpacing * nodeGraphState.CanvasState.ZoomLevel
	};

	// Render frame
	drawList->ChannelsSetCurrent(1);

	const ImColor nodeColor = isNodeSelected ? nodeGraphState.Colors[static_cast<i32>(StyleColor::ColNodeActiveBg)] : nodeGraphState.Colors[static_cast<i32>(StyleColor::ColNodeBg)];
	drawList->AddRectFilled(nodeRect.Min, nodeRect.Max, nodeColor, 5.0f);
	drawList->AddRect(nodeRect.Min, nodeRect.Max, nodeGraphState.Colors[static_cast<i32>(StyleColor::ColNodeBorder)], 5.0f, 15, 2.f);

	// Create node item
	const ImGuiID nodeItemId = ImGui::GetID(nodePointer);
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
		ImGuiID previouslySelectedId = ImHashStr("prev-selected", 0, ImHashData(&CurrentNodeGraph->CurrentNodeInfo.NodePointer, sizeof(CurrentNodeGraph->CurrentNodeInfo.NodePointer)));
		CurrentNodeGraph->CachedData.SetBool(previouslySelectedId, isNodeSelected);
	}

	switch (CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState)
	{
	case SNodeGraphState::CursorState::None:
		isNodeSelected = NodeSelectionBehavior(isNodeSelected);
		break;

	case SNodeGraphState::CursorState::Dragging:
		nodePosition = NodeDraggingBehavior(isNodeSelected, nodePosition);
		break;

	case SNodeGraphState::CursorState::DragSelecting:
		isNodeSelected = DragSelectionBehavior(nodeRect, isNodeSelected);
		break;
	}

	auto* storage = ImGui::GetStateStorage();
	storage->SetFloat(ImGui::GetID("node-width"), ImGui::GetItemRectSize().x);

	ImGui::PopStyleVar();
	ImGui::PopID();
	IdCount--;
}

void WmGui::NodeGraphEditor::Connections(plf::colony<SConnection>& aConnections, NodeGraph& aNodeGraph)
{
	auto it = aConnections.begin();

	while (it != aConnections.end())
	{
		auto& connection = *it;

		if (!WmGui::NodeGraphEditor::Connection(connection.InputNodePointer, connection.InputSlot, connection.OutputNodePointer, connection.OutputSlot, connection.InputSlot->SlotType.GetColor()))
		{
			it = aConnections.erase(it);
			aNodeGraph.MarkDirty();
		}
		else
		{
			it++;
		}
	}
}

void WmGui::NodeGraphEditor::PotentialConnection(NodeGraph& aNodeGraph)
{
	SConnection potentialConnection;

	if (WmGui::NodeGraphEditor::GetNewConnection(&potentialConnection.InputNodePointer, &potentialConnection.OutputNodePointer, &potentialConnection.Color, &potentialConnection.InputSlot, &potentialConnection.OutputSlot))
	{
		auto& connections = aNodeGraph.GetConnections();

		bool shouldAddConnection = true;
		for (auto it = connections.begin(); it != connections.end(); it++)
		{
			if (it->InputNodePointer == potentialConnection.InputNodePointer && it->InputSlot == potentialConnection.InputSlot)
			{
				//[Nicos]: dragging the same connection removes it
				if (it->OutputNodePointer == potentialConnection.OutputNodePointer && it->OutputSlot != potentialConnection.OutputSlot)
					shouldAddConnection = false;

				aNodeGraph.MarkDirty();
				connections.erase(it);
				break;
			}
		}

		if (shouldAddConnection) 
		{
			auto insertedConnectionIt = connections.insert(std::move(potentialConnection));

			insertedConnectionIt->InputSlot->Connection = &*insertedConnectionIt;
			insertedConnectionIt->OutputSlot->Connections.push_back(&*insertedConnectionIt);
			aNodeGraph.MarkDirty();
		}
	}
}

void WmGui::NodeGraphEditor::DrawPendingConnection()
{
	if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
	{
		std::string connection = WmGui::NodeGraphEditor::GetNewNodeConnectionString();
		if (strncmp(payload->DataType, connection.c_str(), connection.size()) == 0)
		{
			auto* dragPayload = (_DragConnectionPayload*)payload->Data;

			ImVec2 slotPosition{
				CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("x", dragPayload->IsInput ? dragPayload->InputSlotInstance->SlotType.Name.c_str() : dragPayload->OutputSlotInstance->SlotType.Name.c_str(), dragPayload->NodePointer,
					dragPayload->IsInput)),
				CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("y",  dragPayload->IsInput ? dragPayload->InputSlotInstance->SlotType.Name.c_str() : dragPayload->OutputSlotInstance->SlotType.Name.c_str(), dragPayload->NodePointer,
					dragPayload->IsInput)),
			};

			//const f32 connectionIndent = 5.f * CurrentNodeGraph->CanvasState.ZoomLevel.x;

			ImVec2 inputPosition, outputPosition;
			if (dragPayload->IsInput)
			{
				inputPosition = slotPosition;
				outputPosition = ImGui::GetMousePos();
			}
			else
			{
				inputPosition = ImGui::GetMousePos();
				outputPosition = slotPosition;
			}

			RenderConnection(inputPosition, outputPosition, 2.f, dragPayload->SlotColor);
		}
	}
}

bool WmGui::NodeGraphEditor::NodeSelectionBehavior(const bool aWasSelected)
{
	auto& io = ImGui::GetIO();
	bool isNodeSelected = aWasSelected;
	auto* nodeId = CurrentNodeGraph->CurrentNodeInfo.NodePointer;
	// Node selection behavior. Selection can change only when no node is being dragged and connections are not being made.
	if (CurrentNodeGraph->JustConnected || ImGui::GetDragDropPayload() != nullptr)
	{
		// No selections are performed when nodes are being connected.
		CurrentNodeGraph->JustConnected = false;
	}
	else if (CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame == ImGui::GetCurrentContext()->FrameCount)
	{
		// Unselect other nodes when some node was left-clicked.
		isNodeSelected = CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodePointer == nodeId;
	}
	else if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && ImGui::IsItemActive())
	{
		isNodeSelected = true;
		if (!io.KeyCtrl && !aWasSelected)
		{
			CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodePointer = nodeId;
			CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
		}
	}
	else if (ImGui::IsMouseReleased(0) && ImGui::IsItemHovered() && ImGui::IsItemActive())
	{
		if (io.KeyAlt)
		{
			isNodeSelected = false;
		}
		else if (!io.KeyCtrl)
		{
			CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodePointer = nodeId;
			CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
		}
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::Dragging;
		if (CurrentNodeGraph->CurrentGraphInfo.DraggedNodePointer == nullptr)
		{
			CurrentNodeGraph->CurrentGraphInfo.DraggedNodePointer = nodeId;
			CurrentNodeGraph->CurrentGraphInfo.ShouldDragAllSelectedNodes = isNodeSelected;
		}
		else
		{

			CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodePointer = nullptr;
		}
	}

	return isNodeSelected;
}

ImVec2 WmGui::NodeGraphEditor::NodeDraggingBehavior(const bool isNodeSelected, const ImVec2 aInitialNodePosition)
{
	ImVec2 newNodePosition = aInitialNodePosition;
	if (ImGui::IsMouseDown(0))
	{
		// Node dragging behavior. Drag node under mouse and other selected nodes if current node is selected.
		if ((ImGui::IsItemActive() || (CurrentNodeGraph->CurrentGraphInfo.DraggedNodePointer && CurrentNodeGraph->CurrentGraphInfo.ShouldDragAllSelectedNodes && isNodeSelected)))
			newNodePosition += ImGui::GetIO().MouseDelta / CurrentNodeGraph->CanvasState.ZoomLevel;
	}

	return newNodePosition;
}

bool WmGui::NodeGraphEditor::DragSelectionBehavior(ImRect nodeRect, const bool aNodeWasInitiallySelected)
{
	bool isSelected = aNodeWasInitiallySelected;
	ImRect selectionArea;
	selectionArea.Min.x = ImMin(CurrentNodeGraph->CurrentGraphInfo.SelectionStart.x, ImGui::GetMousePos().x);
	selectionArea.Min.y = ImMin(CurrentNodeGraph->CurrentGraphInfo.SelectionStart.y, ImGui::GetMousePos().y);
	selectionArea.Max.x = ImMax(CurrentNodeGraph->CurrentGraphInfo.SelectionStart.x, ImGui::GetMousePos().x);
	selectionArea.Max.y = ImMax(CurrentNodeGraph->CurrentGraphInfo.SelectionStart.y, ImGui::GetMousePos().y);

	ImGuiID prevoiusSelectedId = ImHashStr("prev-selected", 0, ImHashData(&CurrentNodeGraph->CurrentNodeInfo.NodePointer, sizeof(CurrentNodeGraph->CurrentNodeInfo.NodePointer)));
	
	if (ImGui::GetIO().KeyCtrl)
	{
		// Append selection
		if (selectionArea.Overlaps(nodeRect))
			isSelected = true;
		else
			isSelected = CurrentNodeGraph->CachedData.GetBool(prevoiusSelectedId);
	}
	else
	{
		// Assign selection
		isSelected = selectionArea.Overlaps(nodeRect);
	}

	return isSelected;
}

void WmGui::NodeGraphEditor::ContextMenu(NodeGraph& aNodeGraph)
{
	const ImVec2 canvasPos = ImGui::GetCurrentWindow()->Pos;

	// Open context menu
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
		ImGui::OpenPopup("context_menu");

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if (ImGui::BeginPopup("context_menu"))
	{
		for (auto& availableNode : aNodeGraph.GetRegisteredNodes())
		{
			//you shouldn't be able to have several root nodes
			if (aNodeGraph.IsNodeTypeRoot(availableNode.NodeType))
				continue;

			if (ImGui::MenuItem(availableNode.Name.c_str(), nullptr, false))
			{
				availableNode.AddNodeLambda(ImGui::GetMousePos() - canvasPos);
			}
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

void WmGui::NodeGraphEditor::UpdateNodeGraphCursor()
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame <= ImGui::GetCurrentContext()->FrameCount)
		CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodePointer = nullptr;

	switch (CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState)
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
						CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodePointer = nullptr;   // unselect all
						CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
					}
				}
			}

			if (ImGui::GetActiveID() == canvasId && ImGui::IsMouseDragging(0))
			{
				CurrentNodeGraph->CurrentGraphInfo.SelectionStart = ImGui::GetMousePos();
				CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::DragSelecting;
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
			CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::None;
			CurrentNodeGraph->CurrentGraphInfo.DraggedNodePointer = nullptr;
		}
		break;
	}
	case SNodeGraphState::CursorState::DragSelecting:
	{
		if (ImGui::IsMouseDown(0))
		{
			drawList->AddRectFilled(CurrentNodeGraph->CurrentGraphInfo.SelectionStart, ImGui::GetMousePos(), CurrentNodeGraph->Colors[static_cast<i32>(StyleColor::ColSelectBg)]);
			drawList->AddRect(CurrentNodeGraph->CurrentGraphInfo.SelectionStart, ImGui::GetMousePos(), CurrentNodeGraph->Colors[static_cast<i32>(StyleColor::ColSelectBorder)]);
		}
		else
		{
			ImGui::ClearActiveID();
			CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::None;
		}
		break;
	}
	}
}

std::string WmGui::NodeGraphEditor::GetNewNodeConnectionString()
{
	std::string name = "NNconnect" + std::to_string(CurrentNodeGraph->Id);

	return name;
}

WmGui::NodeGraphEditor::SNodeGraphState::SNodeGraphState()
{
	auto& imguiStyle = ImGui::GetStyle();
	Colors[static_cast<i32>(StyleColor::ColNodeBg)] = IM_COL32(20, 20, 22, 255);
	Colors[static_cast<i32>(StyleColor::ColSlotEditBg)] = IM_COL32(50, 50, 55, 255);
	Colors[static_cast<i32>(StyleColor::ColSlotEditActiveBg)] = IM_COL32(100, 100, 120, 255);
	Colors[static_cast<i32>(StyleColor::ColNodeActiveBg)] = IM_COL32(85, 85, 110, 255);
	Colors[static_cast<i32>(StyleColor::ColNodeBorder)] = IM_COL32(115, 115, 130, 255);
	Colors[static_cast<i32>(StyleColor::ColSelectBg)] = imguiStyle.Colors[ImGuiCol_FrameBgActive];
	Colors[static_cast<i32>(StyleColor::ColSelectBg)].Value.w = 0.25f;
	Colors[static_cast<i32>(StyleColor::ColSelectBorder)] = imguiStyle.Colors[ImGuiCol_Border];
}
