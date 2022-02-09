#pragma once
#include "NodeGraph/NodeTypes.h"
#include "Imgui/NodeGraphGui.h"
#include <stack>
#include <json/json.hpp>
#include <filesystem>
#include "Resources/Resource.h"
#include "Utilities/plf_colony.h"

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

class NodeGraph : public NonCopyable, public NonMovable, public Resource
{
public:
	NodeGraph() { InitId(); };
	NodeGraph(std::filesystem::path aFilePath);

	void SetNewPath(std::filesystem::path aNewFilePath);

	//[Nicos]: This differs from compile in that it will save after compilation if the flag is set
	void CompileExternal();

	//[Nicos]: This differs from execute in that it will compile before execution if the flag is set
	void ExecuteExternal();

	virtual void Execute();

	void Save();

	void KillNodesThatWantToDie();

	WmGui::NodeGraphEditor::SNodeGraphState& GetNodeGraphGuiState();

	[[nodiscard]] bool IsNodeTypeRoot(const SNodeTypeBase& aNodeType) const;

	[[nodiscard]] size_t GetUniqueId() const { return myUniqueId; }

	plf::colony<SConnection>& GetConnections() { return myConnections; }
	plf::colony<SNode>& GetNodes() { return myNodes; }
	const std::vector<SRegisteredNode>& GetRegisteredNodes() const { return myRegisteredNodes; }

	bool ShouldBeVisible = false;

	void SetName(const std::string& aName) { myName = aName; }
	[[nodiscard]] std::string GetName() const { return myName ? *myName : myPath.string(); }
	[[nodiscard]] virtual std::string GetIdentifier() const { return myPath.string(); }
	[[nodiscard]] std::filesystem::path GetPath() const { return myPath; }

	void Load();

	std::unordered_map<std::string, std::any> myGlobalData;

	inline virtual bool ShouldHotReload() const override { return false; }

	[[nodiscard]] nlohmann::json Serialize();
	void Deserialize(const nlohmann::json& aJsonFile);

	void MarkDirty() { myIsDirty = true; }
	bool IsDirty() const { return myIsDirty; }

protected:
	virtual void Compile();

	std::optional<std::string> myName;
	[[nodiscard]] plf::colony<SNode>::colony_iterator<false> KillNode(plf::colony<SNode>::colony_iterator<false> aIterator);

	virtual void PostLoad() {}

	virtual void FirstTimeSetup();

	void SetupRootNode();

	virtual void RegisterNodes() = 0;

	void SerializeNode(const SNode& aNode, nlohmann::json& aJson);
	void DeserializeNode(const nlohmann::json& aJson);

	void SerializeConnection(SConnection& aConnection, nlohmann::json& aJson);
	void DeserializeConnection(const nlohmann::json& aJson);

	size_t NextNodeId = 0;

	template<typename T>
	[[nodiscard]] SNode& AddNode(ImVec2 InLocation);

	[[nodiscard]] SNode* AddNode(const std::string aNodeType, const ImVec2 aLocation = {0.f, 0.f});

	template<typename T>
	void RegisterNode();

	template<typename T>
	void RegisterRootNode();

	[[nodiscard]] SNode* FindNodeById(const size_t aId);

	void CompileNodeGraph(SNode& aRoot, std::vector<SCompiledNode>& aNodeStack, const bool aIsFirstCompileCall = true);

	SNode* myRootNode = nullptr;

private:
	void InitId();
	void SerializeInlineInputs(SNode& aNode, nlohmann::json& aInputArray);
	void DeserializeInput(const nlohmann::json& aInput);

	std::vector<SCompiledNode> myCompiledNodeStack;
	plf::colony<SConnection> myConnections;
	plf::colony<SNode> myNodes;
	std::vector<SRegisteredNode> myRegisteredNodes;
	std::optional<WmGui::NodeGraphEditor::SNodeGraphState> myNodeGraphGuiState;
	size_t myUniqueId;
	SNodeTypeBase* myRootNodeType = nullptr;
	std::filesystem::path myPath;
	bool myIsDirty = false; 
};

template<typename T>
SNode& NodeGraph::AddNode(const ImVec2 InLocation)
{
	SNode node(_NodeTypes::GetStaticObject<T>());

	node.Id = NextNodeId;
	node.Position = InLocation;
	node.Selected = false;
	node.InputSlotInstances = _NodeTypes::GetStaticObject<T>().CreateInputSlotInstances();
	node.OutputSlotInstances = _NodeTypes::GetStaticObject<T>().CreateOutputSlotInstances();

	NextNodeId++;

	node.NodeGraph = this;

	return *myNodes.insert(std::move(node));
}

template<typename T>
void NodeGraph::RegisterNode()
{
	SRegisteredNode node
	{
		_NodeTypes::GetStaticObject<T>().Title,
		[&](const ImVec2 InLocation) -> SNode& {return AddNode<T>(InLocation); },
		_NodeTypes::GetStaticObject<T>()
	};

	myRegisteredNodes.push_back(node);
}

template<typename T>
void NodeGraph::RegisterRootNode()
{
	RegisterNode<T>();

	myRootNodeType = &_NodeTypes::GetStaticObject<T>();
}

