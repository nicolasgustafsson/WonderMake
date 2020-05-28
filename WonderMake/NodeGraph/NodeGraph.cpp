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

	json empty_array_explicit = jsonFile.array();

	for (SNode& node : Nodes)
	{
		SerializeNode(node, empty_array_explicit);
	}

	jsonFile["Nodes"] = empty_array_explicit;

	return jsonFile;
}

void NodeGraph::Deserialize(const nlohmann::json& aJsonFile)
{
	for (auto& it : aJsonFile["Nodes"].items())
	{
		DeserializeNode(it.value());
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

void NodeGraph::SerializeConnection(SConnection& aNode, nlohmann::json& aJson)
{
	//aJson.push_back(aJson.object({ {"Connection", aNode.NodeType.Title}, {"IsImmortal", aNode.IsImmortal}, {"PositionX", aNode.Position.x}, {"PositionY", aNode.Position.y}, {"Id", aNode.Id} }));
}

void NodeGraph::DeserializeConnection(const nlohmann::json& aJson)
{

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
