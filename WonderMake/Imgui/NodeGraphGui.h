#pragma once
#include "NodeGraph/Node.h"
#include "Canvas.h"

class NodeGraph;
struct SNode;
struct SConnection;
struct SSlotTypeBase;
struct SInputSlotInstanceBase;
struct SOutputSlotInstanceBase;
struct SSlotInstanceBase;

namespace WmGui::NodeGraphEditor
{
	enum StyleColor
	{
		ColNodeBg,
		ColSlotEditBg,
		ColSlotEditActiveBg,
		ColNodeActiveBg,
		ColNodeBorder,
		ColSelectBg,
		ColSelectBorder,
		ColMax
	};

	struct SNodeGraphState
	{
		SNodeGraphState();
		enum class CursorState
		{
			None,
			Dragging,
			DragSelecting
		};

		struct SCurrentNodeInfo
		{
			ImVec2* Position = {};
			void* Id = {};
			bool* Selected = {};
		};

		struct SCurrentSlotInfo
		{
			const char* Title = {};
			bool IsInput = {};
			ImColor Color = {};

			SInputSlotInstanceBase* InputSlotInstance = {};
			SOutputSlotInstanceBase* OutputSlotInstance = {};
		};

		struct SCurrentGraphInfo
		{
			CursorState CurrentCursorState = {};

			/// Starting position of node selection rect.
			ImVec2 selection_start{};
			/// Node id of node that is being dragged.
			void* DraggedNodeId = nullptr;
			/// Flag indicating that all selected nodes should be dragged.
			bool ShouldDragAllSelectedNodes = false;
			/// Node id of node that should be selected on next frame, while deselecting any other nodes.
			void* SingleSelectedNodeId = nullptr;
			//if this matches the current frame, we should try to select things
			i32 DoSelectionsFrame = 0;
		};

		u64 Id;
		bool JustConnected = false;
		SCurrentGraphInfo CurrentGraphInfo = {};
		SCurrentNodeInfo CurrentNodeInfo = {};
		SCurrentSlotInfo CurrentSlotInfo = {};
		SConnection NewConnection = {};
		ImColor Colors[StyleColor::ColMax];

		ImGuiStorage CachedData{};

		SCanvasState CanvasState = {};
	};

	void NodeGraphEdit(NodeGraph& aNodeGraph);

	ImU32 MakeSlotDataID(const char* aData, const char* aSlotTitle, void* aNodeId, bool aInputSlot);

	bool RenderConnection(const ImVec2& aInputPos, const ImVec2& aOutputPos, float aThiccness, const ImColor aColor);

	bool GetNewConnection(void** aInputNodeId, const char** aInputTitle, void** aOutputNodeId, const char** aOutputTitle, ImColor* aColor, SInputSlotInstanceBase** aInput, SOutputSlotInstanceBase** aOutput);

	bool Connection(void* aInputNode, const char* aInputSlot, void* aOutputNode, const char* aOutputSlot, ImColor aColor);

	void Nodes(NodeGraph& aNodeGraph);
	void Node(SNode& aNode);
	plf::colony<SNode>::colony_iterator<false> KillNode(NodeGraph& aNodeGraph, plf::colony<SNode>::colony_iterator<false> aIterator);

	void BeginNode(void* aNodeId, ImVec2* aPosition, bool* aSelected);

	void NodeTitle(const char* aTitle);

	bool IsConnectingCompatibleSlot();
	
	void Slot(const bool aIsInput, SSlotInstanceBase& aSlotInstance); //[Nicos] TODO: make IsInput into slot type instead

	void InputSlots(std::vector<std::unique_ptr<SInputSlotInstanceBase>>& slots);

	void OutputSlots(std::vector<std::unique_ptr<SOutputSlotInstanceBase>>& slots);

	void EndNode();

	void Connections(plf::colony<SConnection>& aConnections);

	void PotentialConnection(NodeGraph& aNodeGraph);

	void DrawPendingConnection();

	bool NodeSelectionBehavior(const bool aWasSelected);

	ImVec2 NodeDraggingBehavior(const bool isNodeSelected, const ImVec2 aInitialNodePosition);

	bool DragSelectionBehavior(ImRect nodeRect, const bool aNodeWasInitiallySelected);

	std::string GetNewNodeConnectionString();

	void ContextMenu(NodeGraph& aNodeGraph);

	void UpdateNodeGraphCursor();
}

