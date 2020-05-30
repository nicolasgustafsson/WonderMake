#pragma once
#include "NodeGraph/NodeTypes.h"
#include "Imgui/NodeGraphGui.h"
#include <stack>
#include <json/json.hpp>
#include <filesystem>

struct SRegisteredNode
{
	std::string Name;
	std::function<SNode& (const ImVec2 InLocation)> AddNodeLambda;
	SNodeTypeBase& NodeType;
};

struct SCompiledNode
{
	SNode& Node;
};

class NodeGraph
{
public:
	NodeGraph(std::filesystem::path aFilePath);

	//[Nicos]: This differs from compile in that it will save after compilation if the flag is set
	void CompileExternal();

	void Save();

	void ExecuteExternal();

	virtual void Execute() {}

	void KillNodesThatWantToDie();

	[[nodiscard]] bool IsNodeTypeRoot(const SNodeTypeBase& aNodeType) const;

	size_t UniqueId;

	plf::colony<SConnection> Connections;
	plf::colony<SNode> Nodes;

	WmGui::NodeGraphEditor::SNodeGraphState NodeGraphGuiState;

	std::vector<SRegisteredNode> RegisteredNodes;

	bool ShouldBeVisible = false;

	std::filesystem::path myPath;

protected:
	virtual void Compile() = 0;

	void Load();

	[[nodiscard]] plf::colony<SNode>::colony_iterator<false> KillNode(plf::colony<SNode>::colony_iterator<false> aIterator);

	virtual void PostLoad() {}

	virtual void FirstTimeSetup();

	void SetupRootNode();

	virtual void RegisterNodes() = 0;
	
	[[nodiscard]] nlohmann::json Serialize();
	void Deserialize(const nlohmann::json& aJsonFile);

	void SerializeNode(const SNode& aNode, nlohmann::json& aJson);
	void DeserializeNode(const nlohmann::json& aJson);

	void SerializeConnection(SConnection& aConnection, nlohmann::json& aJson);
	void DeserializeConnection(const nlohmann::json& aJson);

	size_t NextNodeId = 0;

	template<typename T>
	SNode& AddNode(ImVec2 InLocation);

	SNode* AddNode(const std::string aNodeType, const ImVec2 aLocation = {0.f, 0.f});

	template<typename T>
	void RegisterNode();

	template<typename T>
	void RegisterRootNode();

	[[nodiscard]] SNode* FindNodeById(const size_t aId);

	void CompileNodeGraph(SNode& aRoot, std::vector<SCompiledNode>& aNodeStack, const bool aIsFirstCompileCall = true);

	SNode* myRootNode = nullptr;

private:
	void SerializeInlineInputs(SNode& aNode, nlohmann::json& aInputArray);
	void DeserializeInput(const nlohmann::json& aInput);

	SNodeTypeBase* myRootNodeType = nullptr;
};

template<typename T>
SNode& NodeGraph::AddNode(const ImVec2 InLocation)
{
	SNode node(T::StaticObject);

	node.Id = NextNodeId;
	node.Position = InLocation;
	node.Selected = false;
	node.InputSlotInstances = T::StaticObject.CreateInputSlotInstances();
	node.OutputSlotInstances = T::StaticObject.CreateOutputSlotInstances();

	NextNodeId++;

	return *Nodes.insert(std::move(node));
}

template<typename T>
void NodeGraph::RegisterNode()
{
	SRegisteredNode node
	{
		T::StaticObject.Title,
		[this](const ImVec2 InLocation) -> SNode& {return AddNode<T>(InLocation); },
		T::StaticObject
	};

	RegisteredNodes.push_back(node);
}

template<typename T>
void NodeGraph::RegisterRootNode()
{
	RegisterNode<T>();

	myRootNodeType = &T::StaticObject;
}

