#pragma once
#include "NodeGraph/NodeTypes.h"
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
	enum class StyleColor
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
			SNode* NodePointer = {};
			bool* Selected = {};
		};

		struct SCurrentSlotInfo
		{
			std::string Title = {};
			bool IsInput = {};
			ImColor Color = {};

			SInputSlotInstanceBase* InputSlotInstance = {};
			SOutputSlotInstanceBase* OutputSlotInstance = {};
		};

		struct SCurrentGraphInfo
		{
			CursorState CurrentCursorState = {};

			/// Starting position of node selection rect.
			ImVec2 SelectionStart{};
			/// Node id of node that is being dragged.
			SNode* DraggedNodePointer = nullptr;
			/// Flag indicating that all selected nodes should be dragged.
			bool ShouldDragAllSelectedNodes = false;
			/// Node id of node that should be selected on next frame, while deselecting any other nodes.
			SNode* SingleSelectedNodePointer = nullptr;
			//if this matches the current frame, we should try to select things
			i32 DoSelectionsFrame = 0;
		};

		u64 Id = {};
		bool JustConnected = false;
		SCurrentGraphInfo CurrentGraphInfo = {};
		SCurrentNodeInfo CurrentNodeInfo = {};
		SCurrentSlotInfo CurrentSlotInfo = {};
		SConnection NewConnection = {};
		ImColor Colors[static_cast<i32>(StyleColor::ColMax)];

		ImGuiStorage CachedData = {};

		SCanvasState CanvasState = {};
	};

	void NodeGraphEdit(NodeGraph& aNodeGraph);

	[[nodiscard]] ImU32 MakeSlotDataID(const char* aData, const char* aSlotTitle, SNode* aNodePointer, bool aInputSlot);

	bool RenderConnection(const ImVec2& aInputPos, const ImVec2& aOutputPos, f32 aThiccness, const ImColor aColor);

	//[Nicos]: TODO return optional connection instead of pointer stuffs
	[[nodiscard]] bool GetNewConnection(SNode** aInputNodePointer, SNode** aOutputNodePointer, ImColor* aColor, SInputSlotInstanceBase** aInput, SOutputSlotInstanceBase** aOutput);

	[[nodiscard]] bool Connection(SNode* aInputNode, SInputSlotInstanceBase* aInputSlot, SNode* aOutputNode, SOutputSlotInstanceBase* aOutputSlot, ImColor aColor);

	void Nodes(NodeGraph& aNodeGraph);
	void Node(SNode& aNode);

	void BeginNode(SNode* aNodeId, ImVec2* aPosition, bool* aSelected);

	void NodeTitle(const char* aTitle);

	[[nodiscard]] bool IsConnectingCompatibleSlot();
	
	void Slot(const bool aIsInput, SSlotInstanceBase& aSlotInstance); //[Nicos] TODO: make IsInput into slot type instead

	void InputSlots(std::vector<std::unique_ptr<SInputSlotInstanceBase>>& aSlots);

	void OutputSlots(std::vector<std::unique_ptr<SOutputSlotInstanceBase>>& aSlots);

	void EndNode();

	void Connections(plf::colony<SConnection>& aConnections);

	void PotentialConnection(NodeGraph& aNodeGraph);

	void DrawPendingConnection();

	[[nodiscard]] bool NodeSelectionBehavior(const bool aWasSelected);

	[[nodiscard]] ImVec2 NodeDraggingBehavior(const bool aIsNodeSelected, const ImVec2 aInitialNodePosition);

	[[nodiscard]] bool DragSelectionBehavior(ImRect nodeRect, const bool aNodeWasInitiallySelected);

	[[nodiscard]] std::string GetNewNodeConnectionString();

	void ContextMenu(NodeGraph& aNodeGraph);

	void UpdateNodeGraphCursor();
}

