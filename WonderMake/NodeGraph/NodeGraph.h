#pragma once
#include "NodeGraph/Node.h"
#include "Imgui/NodeGraphGui.h"
#include <stack>

struct SRegisteredNode
{
	std::string Name;
	std::function<void(ImVec2 InLocation)> AddNodeLambda;
	std::function<void(const SNode&)> CompileNodeLambda;
};

struct SCompiledNode
{
	SNode& Node;
};

class NodeGraph
{
public:
	NodeGraph();

	virtual void Compile() = 0;

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
	void RegisterNode(std::function<void(const SNode&)>);

	SNode* FindNodeById(const size_t aId);

	void CompileNodeGraph(SNode& aRoot, std::vector<SCompiledNode>& aNodeStack)
	{
		aNodeStack.push_back({ aRoot });

		for (auto& slotInstance : aRoot.InputSlotInstances)
		{
			if (slotInstance->HasConnection())
			{
				auto id = slotInstance->Connection->OutputNodeId;
				SNode* node = static_cast<SNode*>(id);

				if (node)
				{
					CompileNodeGraph(*node, aNodeStack);
				}
			}
		}
	}

	virtual void Execute() {}
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
void NodeGraph::RegisterNode(std::function<void(const SNode&)> aNodeCompilationLambda)
{
	SRegisteredNode node;
	node.Name = T::StaticObject.Title;
	node.AddNodeLambda = [this](ImVec2 InLocation) {AddNode<T>(InLocation); };
	node.CompileNodeLambda = aNodeCompilationLambda;

	RegisteredNodes.push_back(node);
}

