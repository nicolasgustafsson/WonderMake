#pragma once
#include "NodeGraph/Node.h"
#include "Imgui/NodeGraphGui.h"


struct SRegisteredNode
{
	std::string Name;
	std::function<void(ImVec2 InLocation)> AddNodeLambda;
};

class NodeGraph
{
public:
	NodeGraph();

	u64 UniqueId;

	std::string Name = "Unnamed Node Graph";

	plf::colony<SConnection> Connections;
	plf::colony<SNode> Nodes;

	WmGui::NodeGraphEditor::SNodeGraphState NodeGraphGuiState;

	std::vector<SRegisteredNode> RegisteredNodes;

	bool ShouldBeVisible = false;

protected:
	template<typename T>
	SNode& AddNode(ImVec2 InLocation);

	template<typename T>
	void RegisterNode();
};

template<typename T>
SNode& NodeGraph::AddNode(ImVec2 InLocation)
{
	static size_t index = 0;

	SNode node(T::StaticObject);

	node.Id = index;
	node.Position = InLocation;
	node.Selected = false;
	node.InputSlotInstances = T::StaticObject.CreateInputSlotInstances();
	node.OutputSlotInstances = T::StaticObject.CreateOutputSlotInstances();

	index++;

	return *Nodes.insert(std::move(node));
}

template<typename T>
void NodeGraph::RegisterNode()
{
	SRegisteredNode node;
	node.Name = T::StaticObject.Title;
	node.AddNodeLambda = [this](ImVec2 InLocation) {AddNode<T>(InLocation); };

	RegisteredNodes.push_back(node);
}

