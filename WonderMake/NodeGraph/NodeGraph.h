#pragma once
#include "NodeGraph/Node.h"
#include "Imgui/NodeGraphGui.h"

struct SNode
{
	SNode(SNodeTypeBase& aNodeType)
		: NodeType(aNodeType)
	{

	}

	SNodeTypeBase& NodeType;

	size_t Id;

	ImVec2 Position;

	bool Selected;

	std::vector<std::unique_ptr<SInputSlotInstanceBase>> InputSlotInstances;
	std::vector<std::unique_ptr<SOutputSlotInstanceBase>> OutputSlotInstances;
};

struct SRegisteredNode
{
	std::string Name;
	std::function<void(ImVec2 InLocation)> AddNodeLambda;
};

class NodeGraph
{
public:
	std::string Name = "Unnamed Node Graph";

	plf::colony<SConnection> Connections;
	plf::colony<SNode> Nodes;

	WmGui::SCanvasState CanvasState;

	std::vector<SRegisteredNode> RegisteredNodes;

protected:
	template<typename T>
	void AddNode(ImVec2 InLocation);

	template<typename T>
	void RegisterNode();
};

template<typename T>
void NodeGraph::AddNode(ImVec2 InLocation)
{
	static size_t index = 0;

	SNode node(T::StaticObject);

	node.Id = index;
	node.Position = InLocation;
	node.Selected = false;
	node.InputSlotInstances = T::StaticObject.CreateInputSlotInstances();
	node.OutputSlotInstances = T::StaticObject.CreateOutputSlotInstances();

	Nodes.insert(std::move(node));

	index++;
}

template<typename T>
void NodeGraph::RegisterNode()
{
	SRegisteredNode node;
	node.Name = T::StaticObject.Title;
	node.AddNodeLambda = [this](ImVec2 InLocation) {AddNode<T>(InLocation); };

	RegisteredNodes.push_back(node);
}

