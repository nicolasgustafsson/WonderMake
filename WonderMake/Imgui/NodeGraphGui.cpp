#include "pch.h"
#include "NodeGraphGui.h"
#include "NodeGraph/NodeGraph.h"
#include "NodeGraph/Node.h"

//stack so that we can nest node graphs
static std::vector<WmGui::NodeGraphEditor::SNodeGraphState*> NodeGraphStack;
static WmGui::NodeGraphEditor::SNodeGraphState* CurrentNodeGraph;

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

void WmGui::NodeGraphEditor::NodeGraphEdit(NodeGraph& aNodeGraph)
{
	if (!aNodeGraph.ShouldBeVisible)
		return;

	//[Nicos]: inline unique id into the name as push id doesn't work with begin -- otherwise we would not be able to stack several node graphs with the same name.
	std::string name = aNodeGraph.Name + "###" + std::to_string(aNodeGraph.UniqueId);
	if (ImGui::Begin(name.c_str(), &aNodeGraph.ShouldBeVisible, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Compile"))
			{
				aNodeGraph.Compile();
			}
			ImGui::EndMenuBar();
		}

		CurrentNodeGraph = &aNodeGraph.NodeGraphGuiState;
		CurrentNodeGraph->Id = aNodeGraph.UniqueId;
		NodeGraphStack.push_back(CurrentNodeGraph);

		WmGui::BeginCanvas(&aNodeGraph.NodeGraphGuiState.CanvasState);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 55, 255));
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->ChannelsSplit(4);

		WmGui::NodeGraphEditor::Nodes(aNodeGraph);

		drawList->ChannelsSetCurrent(0);

		WmGui::NodeGraphEditor::Connections(aNodeGraph.Connections);

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
	}


	ImGui::End();
}

ImU32 WmGui::NodeGraphEditor::MakeSlotDataID(const char* aData, const char* aSlotTitle, void* aNodeId, bool aIsInput)
{
	ImU32 SlotId = ImHashStr(aSlotTitle, 0, ImHashData(&aNodeId, sizeof(aNodeId)));
	if (aIsInput)
	{
		// Ensure that input and output slots with same name have different ids.
		SlotId ^= ~0U;
	}
	return ImHashStr(aData, 0, SlotId);
}

bool WmGui::NodeGraphEditor::RenderConnection(const ImVec2& aInputPosition, const ImVec2& aOutputPosition, float aThiccness, const ImColor color)
{
	ImVec2 outDentOffset = { 30.f, 0.f };

	ImVec2 inputOutdent = aInputPosition - outDentOffset;
	ImVec2 outputOutdent = aOutputPosition + outDentOffset;

	ImVec2 direction = outputOutdent - inputOutdent;

	SVector2f directionNormalized = { direction.x, direction.y };
	directionNormalized.Normalize();
	f32 dot = fabsf(directionNormalized.Dot({ 0.f, 1.f }));
	directionNormalized *= dot;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	aThiccness *= CurrentNodeGraph->CanvasState.ZoomLevel;

	auto isCloseLambda = [](ImVec2 aStart, ImVec2 aEnd) {
		const ImVec2 closestPoint = ImLineClosestPoint(aStart, aEnd, ImGui::GetMousePos());
		const float minimumSquareDistance = ImFabs(ImLengthSqr(ImGui::GetMousePos() - closestPoint));
		const bool isClose = minimumSquareDistance <= 200;

		return isClose;
	};

	bool isClose = isCloseLambda(inputOutdent, outputOutdent) || isCloseLambda(aInputPosition, inputOutdent) || isCloseLambda(outputOutdent, aOutputPosition);

	if (!isClose)
		directionNormalized *= 0.5f;

	ImVec2 imDirectionNormalized = { directionNormalized.X, directionNormalized.Y };
	aThiccness = isClose ? aThiccness * 2.f : aThiccness;

	drawList->AddLine(inputOutdent - (imDirectionNormalized * (aThiccness) * 0.5f) + ImVec2{ dot * aThiccness , 0.f }, outputOutdent + (imDirectionNormalized * (aThiccness) * 0.5f) - ImVec2{ dot * aThiccness + 0.2f, 0.f }, color, (aThiccness));
	drawList->AddLine(aInputPosition, inputOutdent + ImVec2{ dot * aThiccness * 1.f, 0.f }, color, aThiccness);
	drawList->AddLine(aOutputPosition, outputOutdent - ImVec2{ dot * aThiccness * 1.f, 0.f }, color, aThiccness);

	return isClose;
}

bool WmGui::NodeGraphEditor::GetNewConnection(void** aInputNodeId, const char** aInputTitle, void** aOutputNodeId, const char** aOutputTitle, ImColor* aColor, SInputSlotInstanceBase** aInputSlotInstance, SOutputSlotInstanceBase** aOutputSlotInstance)
{
	assert(CurrentNodeGraph != nullptr);
	assert(aInputNodeId != nullptr);
	assert(aInputTitle != nullptr);
	assert(aOutputNodeId != nullptr);
	assert(aOutputTitle != nullptr);
	assert(aColor != nullptr);

	auto& canvas = CurrentNodeGraph->CanvasState;
	auto& nodeGraphState = *CurrentNodeGraph;

	if (nodeGraphState.NewConnection.OutputNodeId != nullptr)
	{
		*aInputNodeId = nodeGraphState.NewConnection.InputNodeId;
		*aInputTitle = nodeGraphState.NewConnection.InputSlotName;
		*aOutputNodeId = nodeGraphState.NewConnection.OutputNodeId;
		*aOutputTitle = nodeGraphState.NewConnection.OutputSlotName;
		*aInputSlotInstance = nodeGraphState.NewConnection.InputSlot;
		*aOutputSlotInstance = nodeGraphState.NewConnection.OutputSlot;
		*aColor = nodeGraphState.NewConnection.Color;

		memset(&nodeGraphState.NewConnection, 0, sizeof(nodeGraphState.NewConnection));
		return true;
	}

	return false;
}

bool WmGui::NodeGraphEditor::Connection(void* aInputNode, const char* aInputSlot, void* aOutputNode, const char* aOutputSlot, ImColor color)
{
	assert(CurrentNodeGraph != nullptr);
	assert(aInputNode != nullptr);
	assert(aInputSlot != nullptr);
	assert(aOutputNode != nullptr);
	assert(aOutputSlot != nullptr);

	bool isConnected = true;
	auto& canvas = CurrentNodeGraph->CanvasState;

	ImVec2 input_slot_pos{
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("x", aInputSlot, aInputNode, true)),
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("y", aInputSlot, aInputNode, true)),
	};

	ImVec2 output_slot_pos{
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("x", aOutputSlot, aOutputNode, false)),
		CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("y", aOutputSlot, aOutputNode, false)),
	};

	// Indent connection a bit into slot widget.
	//const float indent =  5.0f * canvas->ZoomLevel;
	//input_slot_pos.x += indent;
	//output_slot_pos.x -= indent;

	bool isCurveHovered = RenderConnection(input_slot_pos, output_slot_pos, 2.0f, color);
	if (isCurveHovered && ImGui::IsWindowHovered())
	{
		ImGuiIO& io = ImGui::GetIO();

		if ((ImGui::IsMouseReleased(0) && io.KeyAlt) || ImGui::IsMouseReleased(2))
			isConnected = false;
	}

	CurrentNodeGraph->CachedData.SetBool(MakeSlotDataID("hovered", aInputSlot, aInputNode, true), isCurveHovered && isConnected);
	CurrentNodeGraph->CachedData.SetBool(MakeSlotDataID("hovered", aOutputSlot, aOutputNode, false), isCurveHovered && isConnected);

	return isConnected;
}

void WmGui::NodeGraphEditor::Node(SNode& aNode)
{
	WmGui::NodeGraphEditor::BeginNode(&aNode, &aNode.Position, &aNode.Selected);
	WmGui::NodeGraphEditor::NodeTitle(aNode.NodeType.Title.c_str());

	ImU32 col = aNode.Selected ? CurrentNodeGraph->Colors[ColSlotEditActiveBg] : CurrentNodeGraph->Colors[ColSlotEditBg];
	ImGui::PushStyleColor(ImGuiCol_FrameBg, col);

	WmGui::NodeGraphEditor::InputSlots(aNode.InputSlotInstances);

	//[Nicos]: TODO add a way to preview stuff here - depending on compiler and node

	WmGui::NodeGraphEditor::OutputSlots(aNode.OutputSlotInstances);

	ImGui::PopStyleColor();

	WmGui::NodeGraphEditor::EndNode();

	ImGuiIO& io = ImGui::GetIO();

	constexpr i32 deleteKey = 261; //[Nicos]: TODO Proper inputs for imgui | ImGuiKey_Delete is incorrect here, but not sure why.

	if (aNode.Selected && io.KeysDown[deleteKey] && !aNode.IsImmortal)
		aNode.IWantToDie = true;
}

void WmGui::NodeGraphEditor::Nodes(NodeGraph& aNodeGraph)
{
	for (auto&& node : aNodeGraph.Nodes)
		WmGui::NodeGraphEditor::Node(node);

	plf::colony<SNode>::colony_iterator<false> it = aNodeGraph.Nodes.begin();

	while (it != aNodeGraph.Nodes.end())
	{
		if (it->IWantToDie)
			it = KillNode(aNodeGraph, it);
		else
			it++;
	}
}

plf::colony<SNode>::colony_iterator<false> WmGui::NodeGraphEditor::KillNode(NodeGraph& aNodeGraph, plf::colony<SNode>::colony_iterator<false> aIterator)
{
	for (auto& inputSlot : aIterator->InputSlotInstances)
	{
		if (inputSlot->Connection)
		{
			auto it = aNodeGraph.Connections.get_iterator_from_pointer(inputSlot->Connection);
			if (it != aNodeGraph.Connections.end())
				aNodeGraph.Connections.erase(it);
		}
	}

	for (auto& outputSlot : aIterator->OutputSlotInstances)
	{
		for (auto&& connection : outputSlot->Connections)
		{
			if (connection)
			{
				auto it = aNodeGraph.Connections.get_iterator_from_pointer(connection);
				if (it != aNodeGraph.Connections.end())
					aNodeGraph.Connections.erase(it);
			}
		}
	}

	return aNodeGraph.Nodes.erase(aIterator);
}

void WmGui::NodeGraphEditor::BeginNode(void* aNodeId, ImVec2* aPosition, bool* aSelected)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	//auto* canvas = gCanvas;
	auto& nodeGraphState = *CurrentNodeGraph;
	//
	nodeGraphState.CurrentNodeInfo.Id = aNodeId;
	nodeGraphState.CurrentNodeInfo.Position = aPosition;
	nodeGraphState.CurrentNodeInfo.Selected = aSelected;

	// 0 - curves and bg
	// 1 - node rect
	// 2 - node hover
	// 3 - node content

	// Top-let corner of the node
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + (*aPosition) * CurrentNodeGraph->CanvasState.ZoomLevel + CurrentNodeGraph->CanvasState.Offset);

	ImGui::PushID(aNodeId);
	IdCount++;

	ImGui::BeginGroup();    // Slots and content group
	drawList->ChannelsSetCurrent(3);
}

void WmGui::NodeGraphEditor::NodeTitle(const char* aTitle)
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

	ImGui::Text(aTitle);

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

bool WmGui::NodeGraphEditor::IsConnectingCompatibleSlot()
{
	assert(CurrentNodeGraph != nullptr);
	auto& nodeGraphState = *CurrentNodeGraph;

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

void WmGui::NodeGraphEditor::Slot(const bool aIsInput, SSlotInstanceBase& aSlotInstance)
{
	CurrentNodeGraph->CurrentSlotInfo.Title = aSlotInstance.SlotType.Name.c_str();
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
	const float circleRadius = 6.5f * CurrentNodeGraph->CanvasState.ZoomLevel;

	ImVec2 titleSize = ImGui::CalcTextSize(aSlotInstance.SlotType.Name.c_str());

	float itemOffsetX = 2.5f + style.ItemSpacing.x * CurrentNodeGraph->CanvasState.ZoomLevel;
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
		if (storage->GetFloat(canvasZoomLevel, CurrentNodeGraph->CanvasState.ZoomLevel) != CurrentNodeGraph->CanvasState.ZoomLevel)
		{
			storage->SetFloat(maxWidthId, 0.0f);
		}
		storage->SetFloat(canvasZoomLevel, CurrentNodeGraph->CanvasState.ZoomLevel);

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
			{
				shouldDrawHollowConnection = false;
			}
		}
	}


	if (shouldDrawHollowConnection)
	{
		drawList->ChannelsSetCurrent(2);

		ImColor nodeBgColor;

		if (CurrentNodeGraph->CurrentNodeInfo.Selected && *(CurrentNodeGraph->CurrentNodeInfo.Selected))
			nodeBgColor = CurrentNodeGraph->Colors[ColNodeActiveBg];
		else
			nodeBgColor = CurrentNodeGraph->Colors[ColNodeBg];

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
	IdCount++;

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
	IdCount--;

	bool shouldDrawHover = false;
	if (ImGui::IsItemHovered())
		shouldDrawHover = true;

	//const ImGuiStyle& style = ImGui::GetStyle();

	ImGui::PushID(CurrentNodeGraph->CurrentSlotInfo.Title);
	IdCount++;

	ImRect slotRect{ ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
	ImGui::ItemAdd(slotRect, ImGui::GetID(CurrentNodeGraph->CurrentSlotInfo.Title));

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

		CurrentNodeGraph->CachedData.SetFloat(MakeSlotDataID("x", CurrentNodeGraph->CurrentSlotInfo.Title, CurrentNodeGraph->CurrentNodeInfo.Id, CurrentNodeGraph->CurrentSlotInfo.IsInput), circleRect.GetCenter().x);
		CurrentNodeGraph->CachedData.SetFloat(MakeSlotDataID("y", CurrentNodeGraph->CurrentSlotInfo.Title, CurrentNodeGraph->CurrentNodeInfo.Id, CurrentNodeGraph->CurrentSlotInfo.IsInput),
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
			dragPayload.NodeId = CurrentNodeGraph->CurrentNodeInfo.Id;
			dragPayload.IsInput = CurrentNodeGraph->CurrentSlotInfo.IsInput;
			dragPayload.SlotTitle = CurrentNodeGraph->CurrentSlotInfo.Title;
			dragPayload.SlotColor = CurrentNodeGraph->CurrentSlotInfo.Color;
			dragPayload.InputSlotInstance = CurrentNodeGraph->CurrentSlotInfo.InputSlotInstance;
			dragPayload.OutputSlotInstance = CurrentNodeGraph->CurrentSlotInfo.OutputSlotInstance;

			ImGui::SetDragDropPayload(connectionStr.c_str(), &dragPayload, sizeof(dragPayload));

			// Clear new connection info
			CurrentNodeGraph->NewConnection.InputNodeId = nullptr;
			CurrentNodeGraph->NewConnection.InputSlot = nullptr;
			CurrentNodeGraph->NewConnection.OutputNodeId = nullptr;
			CurrentNodeGraph->NewConnection.OutputSlot = nullptr;
			CurrentNodeGraph->NewConnection.InputSlotName = nullptr;
			CurrentNodeGraph->NewConnection.OutputSlotName = nullptr;
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

		if (payload = ImGui::AcceptDragDropPayload(connectionStr.c_str(), ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			auto* dragData = (_DragConnectionPayload*)payload->Data;

			// Store info of source slot to be queried by ImNodes::GetConnection()
			if (!CurrentNodeGraph->CurrentSlotInfo.IsInput)
			{
				CurrentNodeGraph->NewConnection.InputNodeId = dragData->NodeId;
				CurrentNodeGraph->NewConnection.InputSlotName = dragData->SlotTitle;
				CurrentNodeGraph->NewConnection.InputSlot = dragData->InputSlotInstance;
				CurrentNodeGraph->NewConnection.OutputNodeId = CurrentNodeGraph->CurrentNodeInfo.Id;
				CurrentNodeGraph->NewConnection.OutputSlotName = CurrentNodeGraph->CurrentSlotInfo.Title;
				CurrentNodeGraph->NewConnection.OutputSlot = CurrentNodeGraph->CurrentSlotInfo.OutputSlotInstance;
				CurrentNodeGraph->NewConnection.Color = dragData->SlotColor;
			}
			else
			{
				CurrentNodeGraph->NewConnection.InputNodeId = CurrentNodeGraph->CurrentNodeInfo.Id;
				CurrentNodeGraph->NewConnection.InputSlotName = CurrentNodeGraph->CurrentSlotInfo.Title;
				CurrentNodeGraph->NewConnection.InputSlot = CurrentNodeGraph->CurrentSlotInfo.InputSlotInstance;
				CurrentNodeGraph->NewConnection.OutputNodeId = dragData->NodeId;
				CurrentNodeGraph->NewConnection.OutputSlotName = dragData->SlotTitle;
				CurrentNodeGraph->NewConnection.Color = dragData->SlotColor;
				CurrentNodeGraph->NewConnection.OutputSlot = dragData->OutputSlotInstance;
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
			aSlotInstance.Inspect();
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

void WmGui::NodeGraphEditor::InputSlots(std::vector<std::unique_ptr<SInputSlotInstanceBase>>& slots)
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

void WmGui::NodeGraphEditor::OutputSlots(std::vector<std::unique_ptr<SOutputSlotInstanceBase>>& slots)
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

void WmGui::NodeGraphEditor::EndNode()
{
	//assert(gCanvas != nullptr);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 5.f, 5.f });
	const ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	//auto* canvas = gCanvas;
	auto& nodeGraphState = *CurrentNodeGraph;
	auto* nodeId = nodeGraphState.CurrentNodeInfo.Id;
	//
	bool& isNodeSelected = *nodeGraphState.CurrentNodeInfo.Selected;
	ImVec2& nodePosition = *nodeGraphState.CurrentNodeInfo.Position;
	//

	ImGui::EndGroup();    // Slots and content group

	ImRect nodeRect{
		ImGui::GetItemRectMin() - style.ItemInnerSpacing * nodeGraphState.CanvasState.ZoomLevel,
		ImGui::GetItemRectMax() + style.ItemInnerSpacing * nodeGraphState.CanvasState.ZoomLevel
	};

	// Render frame
	drawList->ChannelsSetCurrent(1);

	ImColor nodeColor = isNodeSelected ? nodeGraphState.Colors[ColNodeActiveBg] : nodeGraphState.Colors[ColNodeBg];
	drawList->AddRectFilled(nodeRect.Min, nodeRect.Max, nodeColor, 5.0f);
	drawList->AddRect(nodeRect.Min, nodeRect.Max, nodeGraphState.Colors[ColNodeBorder], 5.0f, 15, 2.f);

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
		ImGuiID previouslySelectedId = ImHashStr("prev-selected", 0, ImHashData(&CurrentNodeGraph->CurrentNodeInfo.Id, sizeof(CurrentNodeGraph->CurrentNodeInfo.Id)));
		CurrentNodeGraph->CachedData.SetBool(previouslySelectedId, isNodeSelected);
	}

	ImGuiIO& io = ImGui::GetIO();
	switch (CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState)
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
	IdCount--;
}

void WmGui::NodeGraphEditor::Connections(plf::colony<SConnection>& aConnections)
{
	auto it = aConnections.begin();

	while (it != aConnections.end())
	{
		auto& connection = *it;
		auto& style = ImGui::GetStyle();
		if (!WmGui::NodeGraphEditor::Connection(connection.InputNodeId, connection.InputSlotName, connection.OutputNodeId, connection.OutputSlotName, connection.Color))
		{
			it = aConnections.erase(it);
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

	if (WmGui::NodeGraphEditor::GetNewConnection(&potentialConnection.InputNodeId, &potentialConnection.InputSlotName, &potentialConnection.OutputNodeId, &potentialConnection.OutputSlotName, &potentialConnection.Color, &potentialConnection.InputSlot, &potentialConnection.OutputSlot))
	{
		bool shouldAddConnection = true;
		for (auto it = aNodeGraph.Connections.begin(); it != aNodeGraph.Connections.end(); it++)
		{
			if (it->InputNodeId == potentialConnection.InputNodeId && strcmp(it->InputSlotName, potentialConnection.InputSlotName) == 0)
			{
				//[Nicos]: dragging the same connection should remove it
				if (it->OutputNodeId == potentialConnection.OutputNodeId && strcmp(it->OutputSlotName, potentialConnection.OutputSlotName) == 0)
					shouldAddConnection = false;

				aNodeGraph.Connections.erase(it);
				break;
			}
		}

		if (shouldAddConnection)
		{
			auto insertedConnectionIt = aNodeGraph.Connections.insert(std::move(potentialConnection));

			insertedConnectionIt->InputSlot->Connection = &*insertedConnectionIt;
			insertedConnectionIt->OutputSlot->Connections.push_back(&*insertedConnectionIt);
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
				CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("x", dragPayload->SlotTitle, dragPayload->NodeId,
					dragPayload->IsInput)),
				CurrentNodeGraph->CachedData.GetFloat(MakeSlotDataID("y", dragPayload->SlotTitle, dragPayload->NodeId,
					dragPayload->IsInput)),
			};

			const float connectionIndent = 5.f * CurrentNodeGraph->CanvasState.ZoomLevel;

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

bool WmGui::NodeGraphEditor::NodeSelectionBehavior(const bool aWasSelected)
{
	auto& io = ImGui::GetIO();
	bool isNodeSelected = aWasSelected;
	auto* nodeId = CurrentNodeGraph->CurrentNodeInfo.Id;
	// Node selection behavior. Selection can change only when no node is being dragged and connections are not being made.
	if (CurrentNodeGraph->JustConnected || ImGui::GetDragDropPayload() != nullptr)
	{
		// No selections are performed when nodes are being connected.
		CurrentNodeGraph->JustConnected = false;
	}
	else if (CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame == ImGui::GetCurrentContext()->FrameCount)
	{
		// Unselect other nodes when some node was left-clicked.
		isNodeSelected = CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodeId == nodeId;
	}
	else if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && ImGui::IsItemActive())
	{
		isNodeSelected = true;
		if (!io.KeyCtrl && !aWasSelected)
		{
			CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodeId = nodeId;
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
			CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodeId = nodeId;
			CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
		}
	}
	else if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		CurrentNodeGraph->CurrentGraphInfo.CurrentCursorState = SNodeGraphState::CursorState::Dragging;
		if (CurrentNodeGraph->CurrentGraphInfo.DraggedNodeId == nullptr)
		{
			CurrentNodeGraph->CurrentGraphInfo.DraggedNodeId = nodeId;
			CurrentNodeGraph->CurrentGraphInfo.ShouldDragAllSelectedNodes = isNodeSelected;
		}
		else
		{

			CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodeId = nullptr;
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
		if ((ImGui::IsItemActive() || (CurrentNodeGraph->CurrentGraphInfo.DraggedNodeId && CurrentNodeGraph->CurrentGraphInfo.ShouldDragAllSelectedNodes && isNodeSelected)))
			newNodePosition += ImGui::GetIO().MouseDelta / CurrentNodeGraph->CanvasState.ZoomLevel;
	}

	return newNodePosition;
}

bool WmGui::NodeGraphEditor::DragSelectionBehavior(ImRect nodeRect, const bool aNodeWasInitiallySelected)
{
	bool isSelected = aNodeWasInitiallySelected;
	ImRect selectionArea;
	selectionArea.Min.x = ImMin(CurrentNodeGraph->CurrentGraphInfo.selection_start.x, ImGui::GetMousePos().x);
	selectionArea.Min.y = ImMin(CurrentNodeGraph->CurrentGraphInfo.selection_start.y, ImGui::GetMousePos().y);
	selectionArea.Max.x = ImMax(CurrentNodeGraph->CurrentGraphInfo.selection_start.x, ImGui::GetMousePos().x);
	selectionArea.Max.y = ImMax(CurrentNodeGraph->CurrentGraphInfo.selection_start.y, ImGui::GetMousePos().y);

	ImGuiID prevoiusSelectedId = ImHashStr("prev-selected", 0, ImHashData(&CurrentNodeGraph->CurrentNodeInfo.Id, sizeof(CurrentNodeGraph->CurrentNodeInfo.Id)));
	if (ImGui::GetIO().KeyShift)
	{
		// Append selection
		if (selectionArea.Overlaps(nodeRect))
			isSelected = true;
		else
			isSelected = CurrentNodeGraph->CachedData.GetBool(prevoiusSelectedId);
	}
	else if (ImGui::GetIO().KeyCtrl)
	{

		// Append selection
		if (selectionArea.Overlaps(nodeRect))
			isSelected = true;
		else
			isSelected = CurrentNodeGraph->CachedData.GetBool(prevoiusSelectedId);

		//// Subtract from selection
		//if (selectionArea.Overlaps(nodeRect))
		//	isSelected = false;
		//else
		//	isSelected = CurrentNodeGraph->CachedData.GetBool(prevoiusSelectedId);
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
	bool shouldOpenContextMenu = false;

	const ImVec2 canvasPos = ImGui::GetCurrentWindow()->Pos;

	// Open context menu
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
		shouldOpenContextMenu = true;

	if (shouldOpenContextMenu)
		ImGui::OpenPopup("context_menu");

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if (ImGui::BeginPopup("context_menu"))
	{
		for (auto& availableNode : aNodeGraph.RegisteredNodes)
		{
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
		CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodeId = nullptr;

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
						CurrentNodeGraph->CurrentGraphInfo.SingleSelectedNodeId = nullptr;   // unselect all
						CurrentNodeGraph->CurrentGraphInfo.DoSelectionsFrame = ImGui::GetCurrentContext()->FrameCount + 1;
					}
				}
			}

			if (ImGui::GetActiveID() == canvasId && ImGui::IsMouseDragging(0))
			{
				CurrentNodeGraph->CurrentGraphInfo.selection_start = ImGui::GetMousePos();
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
			CurrentNodeGraph->CurrentGraphInfo.DraggedNodeId = nullptr;
		}
		break;
	}
	case SNodeGraphState::CursorState::DragSelecting:
	{
		if (ImGui::IsMouseDown(0))
		{
			drawList->AddRectFilled(CurrentNodeGraph->CurrentGraphInfo.selection_start, ImGui::GetMousePos(), CurrentNodeGraph->Colors[ColSelectBg]);
			drawList->AddRect(CurrentNodeGraph->CurrentGraphInfo.selection_start, ImGui::GetMousePos(), CurrentNodeGraph->Colors[ColSelectBorder]);
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
	Colors[ColNodeBg] = IM_COL32(20, 20, 22, 255);
	Colors[ColSlotEditBg] = IM_COL32(50, 50, 55, 255);
	Colors[ColSlotEditActiveBg] = IM_COL32(100, 100, 120, 255);
	Colors[ColNodeActiveBg] = IM_COL32(85, 85, 110, 255);
	Colors[ColNodeBorder] = IM_COL32(115, 115, 130, 255);
	Colors[ColSelectBg] = imguiStyle.Colors[ImGuiCol_FrameBgActive];
	Colors[ColSelectBg].Value.w = 0.25f;
	Colors[ColSelectBorder] = imguiStyle.Colors[ImGuiCol_Border];
}
