#include "pch.h"
#include "NodeGraph.h"

NodeGraph::NodeGraph(std::filesystem::path aFilePath)
	: myPath(aFilePath)
{
	static u64 uniqueIdCounter = 0;
	UniqueId = uniqueIdCounter;
	uniqueIdCounter++;
}

void NodeGraph::Save()
{
	nlohmann::json json = Serialize();

	std::ofstream file(myPath);

	file << std::setw(4) << json.dump();
}

void NodeGraph::Load()
{
	RegisterNodes();

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
}

nlohmann::json NodeGraph::Serialize()
{
	nlohmann::json jsonFile;

	json nodeArray = jsonFile.array();

	for (SNode& node : Nodes)
	{
		SerializeNode(node, nodeArray);
	}

	jsonFile["Nodes"] = nodeArray;

	json connectionArray = jsonFile.array();

	for (SConnection& connection : Connections)
	{
		SerializeConnection(connection, connectionArray);
	}

	jsonFile["Connections"] = connectionArray;

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
}

void NodeGraph::SerializeNode(SNode& aNode, nlohmann::json& aJson)
{
	aJson.push_back(aJson.object({ {"NodeType", aNode.NodeType.Title}, {"IsImmortal", aNode.IsImmortal}, {"PositionX", aNode.Position.x}, {"PositionY", aNode.Position.y}, {"Id", aNode.Id} }));
}

void NodeGraph::DeserializeNode(const nlohmann::json& aJson)
{
	SNode* node = AddNode(aJson["NodeType"].get<std::string>());

	if (node)
	{
		node->IsImmortal = aJson["IsImmortal"].get<bool>();
		node->Position.x = aJson["PositionX"].get<f32>();
		node->Position.y = aJson["PositionY"].get<f32>();
		node->Id = aJson["Id"].get<size_t>();

		if (node->Id >= NextNodeIndex)
			NextNodeIndex = node->Id + 1;
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

	connection.Color = connection.InputSlot->SlotType.GetColor();

	SConnection& connectionRef = *Connections.insert(std::move(connection));

	connectionRef.OutputSlot->Connections.push_back(&connectionRef);
	connectionRef.InputSlot->Connection = &connectionRef;
}

SNode* NodeGraph::AddNode(std::string NodeType)
{
	for (auto& registeredNode : RegisteredNodes)
	{
		if (registeredNode.Name == NodeType)
		{
			return &registeredNode.AddNodeLambda({});
		}
	}

	return nullptr;
}

SNode* NodeGraph::FindNodeById(const size_t aId)
{
	for (SNode& node : Nodes)
	{
		if (node.Id == aId)
			return &node;
	}

	return nullptr;
}

void NodeGraph::CompileNodeGraph(SNode& aRoot, std::vector<SCompiledNode>& aNodeStack)
{
	aNodeStack.push_back({ aRoot });

	for (auto& slotInstance : aRoot.InputSlotInstances)
	{
		if (slotInstance->HasConnection())
		{
			auto id = slotInstance->Connection->OutputNodePointer;
			SNode* node = static_cast<SNode*>(id);

			if (node)
			{
				CompileNodeGraph(*node, aNodeStack);
			}
		}
	}
}
