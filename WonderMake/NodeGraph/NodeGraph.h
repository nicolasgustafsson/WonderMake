#pragma once
#include "NodeGraph/Node.h"
#include "Imgui/NodeGraphGui.h"
#include <stack>
#include <json/json.hpp>
#include <filesystem>

struct SRegisteredNode
{
	std::string Name;
	std::function<SNode& (ImVec2 InLocation)> AddNodeLambda;
};

struct SCompiledNode
{
	SNode& Node;
};

class NodeGraph
{
public:
	NodeGraph(std::filesystem::path aFilePath);

	virtual void Compile() = 0;

	void Save();
	void Load();

	size_t UniqueId;

	std::string Name = "Unnamed Node Graph";

	plf::colony<SConnection> Connections;
	plf::colony<SNode> Nodes;

	WmGui::NodeGraphEditor::SNodeGraphState NodeGraphGuiState;

	std::vector<SRegisteredNode> RegisteredNodes;

	bool ShouldBeVisible = false;

protected:
	virtual void PostLoad() {}

	virtual void FirstTimeSetup() {};
	virtual void RegisterNodes() = 0;
	
	nlohmann::json Serialize();
	void Deserialize(const nlohmann::json& aJsonFile);

	void SerializeNode(SNode& aNode, nlohmann::json& aJson);
	void DeserializeNode(const nlohmann::json& aJson);

	void SerializeConnection(SConnection& aConnection, nlohmann::json& aJson);
	void DeserializeConnection(const nlohmann::json& aJson);

	size_t NextNodeIndex = 0;

	std::filesystem::path myPath;

	template<typename T>
	SNode& AddNode(ImVec2 InLocation);

	SNode* AddNode(std::string NodeType);

	template<typename T>
	void RegisterNode();

	SNode* FindNodeById(const size_t aId);

	void CompileNodeGraph(SNode& aRoot, std::vector<SCompiledNode>& aNodeStack);

	virtual void Execute() {}
private:
	void SerializeInlineInputs(SNode& aNode, nlohmann::json& aInputArray);
	void DeserializeInput(const nlohmann::json& aInput);
};

template<typename T>
SNode& NodeGraph::AddNode(ImVec2 InLocation)
{
	SNode node(T::StaticObject);

	node.Id = NextNodeIndex;
	node.Position = InLocation;
	node.Selected = false;
	node.InputSlotInstances = T::StaticObject.CreateInputSlotInstances();
	node.OutputSlotInstances = T::StaticObject.CreateOutputSlotInstances();

	NextNodeIndex++;

	return *Nodes.insert(std::move(node));
}

template<typename T>
void NodeGraph::RegisterNode()
{
	SRegisteredNode node;
	node.Name = T::StaticObject.Title;
	node.AddNodeLambda = [this](ImVec2 InLocation) -> SNode& {return AddNode<T>(InLocation); };

	RegisteredNodes.push_back(node);
}

