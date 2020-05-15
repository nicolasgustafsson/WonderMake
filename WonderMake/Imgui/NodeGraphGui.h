#pragma once
#include "NodeGraph/Node.h"

class NodeGraph;
struct SNode;
struct SConnection;
struct SSlotTypeBase;
struct SInputSlotInstanceBase;
struct SOutputSlotInstanceBase;
struct SSlotInstanceBase;

namespace WmGui
{
	enum StyleColor
	{
		ColNodeBg,
		ColNodeActiveBg,
		ColNodeBorder,
		ColSelectBg,
		ColSelectBorder,
		ColMax
	};


	struct SNodeGraphState
	{
		enum class CursorState
		{
			None,
			Dragging,
			DragSelecting
		};

		struct SCurrentNodeInfo
		{
			ImVec2* Position;
			void* Id;
			bool* Selected;
		};

		struct SCurrentSlotInfo
		{
			const char* Title;
			bool IsInput;
			ImColor Color;

			SInputSlotInstanceBase* InputSlotInstance;
			SOutputSlotInstanceBase* OutputSlotInstance;
		};

		struct SCurrentGraphInfo
		{
			CursorState CurrentCursorState;

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

		bool JustConnected = false;
		SCurrentGraphInfo CurrentGraphInfo;
		SCurrentNodeInfo CurrentNodeInfo;
		SCurrentSlotInfo CurrentSlotInfo;
		SConnection NewConnection;
	};

	struct SCanvasState
	{
		SCanvasState();
		ImColor colors[StyleColor::ColMax];
		ImGuiStorage CachedData{};

		f32 ZoomLevel = 1;
		ImVec2 Offset = { 0.f, 0.f };

		//[Nicos]: May want to separate this from canvas in the future
		SNodeGraphState NodeGraphState;
	};

	void NodeGraphEditor(NodeGraph& aNodeGraph, bool* aShouldShow);

	ImU32 MakeSlotDataID(const char* data, const char* slot_title, void* node_id, bool input_slot);

	bool RenderConnection(const ImVec2& input_pos, const ImVec2& output_pos, float thickness, const ImColor aColor);

	bool GetNewConnection(void** aInputNodeId, const char** aInputTitle, void** aOutputNodeId, const char** aOutputTitle, ImColor* aColor, SInputSlotInstanceBase** aInput, SOutputSlotInstanceBase** aOutput);

	bool Connection(void* input_node, const char* input_slot, void* output_node, const char* output_slot, ImColor color);

	void BeginCanvas(SCanvasState* aCanvasState);

	void EndCanvas();

	void Nodes(plf::colony<SNode>& aNodes);
	void Node(SNode& node);

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

	void ContextMenu(NodeGraph& aNodeGraph);
}

