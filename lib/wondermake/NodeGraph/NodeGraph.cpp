#include "pch.h"
#include "NodeGraph.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/SystemPtr.h"
#include "wondermake-base/WmLogTags.h"

NodeGraph::NodeGraph(FilePath aFilePath)
	: myPath(std::move(aFilePath))
{
	static size_t uniqueIdCounter = 0;
	myUniqueId = uniqueIdCounter;
	uniqueIdCounter++;
}

NodeGraph::~NodeGraph()
{
	myConnections.clear();
}

void NodeGraph::SetNewPath(FilePath aNewFilePath)
{
	myPath = std::move(aNewFilePath);

	Load();
}

void NodeGraph::CompileExternal()
{
	Compile();

	SystemPtr<DebugSettingsSystem> debugSettings;

	const bool shouldSave = debugSettings->GetOrCreateDebugValue("NodeGraph/Should save on compile", true);

	if (shouldSave)
		Save();
}

void NodeGraph::Save()
{
	nlohmann::json json = Serialize();

	std::ofstream file(myPath);

	file << std::setw(4) << json.dump();

	WmLogSuccess(TagWonderMake << TagWmNodeGraph << "Saved node graph [" << myPath << "].");
}

void NodeGraph::ExecuteExternal()
{
	SystemPtr<DebugSettingsSystem> debugSettings;

	const bool shouldCompile = debugSettings->GetOrCreateDebugValue("NodeGraph/Should compile before execute", true);

	if (shouldCompile)
		Compile();

	Execute();
}

void NodeGraph::Load()
{
	RegisterNodes();

	if (myRootNodeType == nullptr)
		WmLogError(TagWonderMake << TagWmNodeGraph << "No root node type registered! Things may break.");

	std::ifstream file(myPath, std::fstream::app);

	std::string fileContents((std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));

	nlohmann::json jsonFile;

	if (json::accept(fileContents))
	{
		jsonFile = json::parse(fileContents);
		Deserialize(jsonFile);
	}
	else
	{
		FirstTimeSetup();
	}

	PostLoad();
}

void NodeGraph::Compile()
{
	myCompiledNodeStack.clear();

	CompileNodeGraph(*myRootNode, myCompiledNodeStack);
	myNeedsRecompile = false;
}

void NodeGraph::Execute()
{
	if (myNeedsRecompile)
		Compile();

	for (auto it = myCompiledNodeStack.rbegin(); it != myCompiledNodeStack.rend(); it++)
	{
		it->Node.NodeType.ExecuteNode(it->Node);
	}
}

void NodeGraph::KillNodesThatWantToDie()
{
	plf::colony<SNode>::colony_iterator<false> it = myNodes.begin();

	while (it != myNodes.end())
	{
		//[Nicos]: Root nodes can't die even if they want to
		if (it->IWantToDie && !IsNodeTypeRoot(it->NodeType))
			it = KillNode(it);
		else
			it++;
	}
}

WmGui::NodeGraphEditor::SNodeGraphState& NodeGraph::GetNodeGraphGuiState()
{
	if (!myNodeGraphGuiState)
		myNodeGraphGuiState.emplace();

	return *myNodeGraphGuiState;
}

bool NodeGraph::IsNodeTypeRoot(const SNodeTypeBase& aNodeType) const
{
	return &aNodeType == myRootNodeType;
}

plf::colony<SNode>::colony_iterator<false> NodeGraph::KillNode(plf::colony<SNode>::colony_iterator<false> aIterator)
{
	for (auto& inputSlot : aIterator->InputSlotInstances)
	{
		if (inputSlot->Connection)
		{
			auto it = myConnections.get_iterator_from_pointer(inputSlot->Connection);
			if (it != myConnections.end())
				myConnections.erase(it);
		}
	}

	for (auto& outputSlot : aIterator->OutputSlotInstances)
	{
		for (auto&& connection : outputSlot->Connections)
		{
			if (connection)
			{
				auto it = myConnections.get_iterator_from_pointer(connection);
				if (it != myConnections.end())
					myConnections.erase(it);
			}
		}
	}

	myNeedsRecompile = true;
	return myNodes.erase(aIterator);
}

void NodeGraph::FirstTimeSetup()
{
	SetupRootNode();
}

void NodeGraph::SetupRootNode()
{
	if (!myRootNodeType)
		return;

	myRootNode = AddNode(myRootNodeType->Title, { 1000.f, 300.f });
}

nlohmann::json NodeGraph::Serialize()
{
	nlohmann::json jsonFile;

	json nodeArray = jsonFile.array();

	for (SNode& node : myNodes)
	{
		SerializeNode(node, nodeArray);
	}

	jsonFile["Nodes"] = nodeArray;

	json connectionArray = jsonFile.array();

	for (SConnection& connection : myConnections)
	{
		SerializeConnection(connection, connectionArray);
	}

	jsonFile["Connections"] = connectionArray;

	json inlineValueArray = jsonFile.array();

	for (SNode& node : myNodes)
	{
		SerializeInlineInputs(node, inlineValueArray);
	}

	jsonFile["InlineInputValues"] = inlineValueArray;

	return jsonFile;
}

void NodeGraph::Deserialize(const nlohmann::json& aJsonFile)
{
	for (auto& it : aJsonFile["Nodes"].items())
	{
		DeserializeNode(it.value());
	}

	for (auto& it : aJsonFile["Connections"].items())
	{
		DeserializeConnection(it.value());
	}

	for (auto& it : aJsonFile["InlineInputValues"].items())
	{
		DeserializeInput(it.value());
	}

	if (!myRootNode && myRootNodeType)
	{
		for (auto& node : myNodes)
		{
			if (&node.NodeType == myRootNodeType)
			{
				myRootNode = &node;
				break;
			}
		}

		if (!myRootNode)
		{
			WmLogError(TagWonderMake << TagWmNodeGraph << "No root node detected on load! Adding one...");
			SetupRootNode();
		}
	}
}

void NodeGraph::SerializeNode(const SNode& aNode, nlohmann::json& aJson)
{
	aJson.push_back(aJson.object({ {"NodeType", aNode.NodeType.Title}, {"PositionX", aNode.Position.x}, {"PositionY", aNode.Position.y}, {"Id", aNode.Id} }));
}

void NodeGraph::DeserializeNode(const nlohmann::json& aJson)
{
	SNode* node = AddNode(aJson["NodeType"].get<std::string>());

	if (node)
	{
		node->Position.x = aJson["PositionX"].get<f32>();
		node->Position.y = aJson["PositionY"].get<f32>();
		node->Id = aJson["Id"].get<size_t>();

		//keep node index updated so that we don't accidentally add a new node with an existing id
		if (node->Id >= NextNodeId)
			NextNodeId = node->Id + 1;
	}
}

void NodeGraph::SerializeConnection(SConnection& aConnection, nlohmann::json& aJson)
{
	const std::optional<i32> outputSlot = aConnection.OutputNodePointer->GetIndexOfOutputSlot(aConnection.OutputSlot);
	const std::optional<i32> inputSlot = aConnection.InputNodePointer->GetIndexOfInputSlot(aConnection.InputSlot);
	aJson.push_back(aJson.object({ {"NodeFrom", aConnection.OutputNodePointer->Id},{"NodeTo", aConnection.InputNodePointer->Id}, {"SlotFrom", outputSlot ? *outputSlot : 0}, {"SlotTo", inputSlot ? *inputSlot : 0} }));
}

void NodeGraph::DeserializeConnection(const nlohmann::json& aJson)
{
	SConnection connection;
	
	const i32 nodeFromId = aJson["NodeFrom"].get<i32>();
	const i32 nodeToId = aJson["NodeTo"].get<i32>();

	connection.OutputNodePointer = FindNodeById(nodeFromId);
	connection.InputNodePointer = FindNodeById(nodeToId);

	if (!connection.OutputNodePointer)
		return;
	if (!connection.InputNodePointer)
		return;

	const i32 slotFromId = aJson["SlotFrom"].get<i32>();
	const i32 slotToId = aJson["SlotTo"].get<i32>();

	connection.OutputSlot = connection.OutputNodePointer->OutputSlotInstances[slotFromId].get();
	connection.InputSlot = connection.InputNodePointer->InputSlotInstances[slotToId].get();


	SConnection& connectionRef = *myConnections.insert(std::move(connection));

	connectionRef.OutputSlot->Connections.push_back(&connectionRef);
	connectionRef.InputSlot->Connection = &connectionRef;
}

SNode* NodeGraph::AddNode(const std::string aNodeType, const ImVec2 aLocation)
{
	for (auto& registeredNode : myRegisteredNodes)
	{
		if (registeredNode.Name == aNodeType)
		{
			return &registeredNode.AddNodeLambda(aLocation);
		}
	}

	return nullptr;
}

SNode* NodeGraph::FindNodeById(const size_t aId)
{
	for (SNode& node : myNodes)
	{
		if (node.Id == aId)
			return &node;
	}

	return nullptr;
}

void NodeGraph::CompileNodeGraph(SNode& aRoot, std::vector<SCompiledNode>& aNodeStack, const bool aIsFirstCompileCall)
{
	aNodeStack.push_back({ aRoot });

	for (auto& slotInstance : aRoot.InputSlotInstances)
	{
		if (slotInstance->HasConnection())
		{
			auto id = slotInstance->Connection->OutputNodePointer;
			SNode* node = static_cast<SNode*>(id);

			node->ClearNodeData();

			if (node)
				CompileNodeGraph(*node, aNodeStack, false);
		}
	}

	if (aIsFirstCompileCall)
		WmLogSuccess(TagWonderMake << TagWmNodeGraph << "Compiled node graph [" << myPath << "].");
}

void NodeGraph::SerializeInlineInputs(SNode& aNode, json& aInputArray)
{
	for (size_t i = 0; i < aNode.InputSlotInstances.size(); i++)
	{
		auto* inputSlotInstance = aNode.InputSlotInstances[i].get();

		if (inputSlotInstance->HasConnection())
			continue;

		inputSlotInstance->SerializeInlineInput(static_cast<i32>(aNode.Id), static_cast<i32>(i), aInputArray);
	}
}

void NodeGraph::DeserializeInput(const nlohmann::json& aInput)
{
	const i32 nodeId = aInput["NodeId"].get<i32>();
	const i32 slotId = aInput["SlotId"].get<i32>();

	SNode* node = FindNodeById(nodeId);

	if (!node)
		return;

	node->InputSlotInstances[slotId]->DeserializeInlineInput(aInput);

}
