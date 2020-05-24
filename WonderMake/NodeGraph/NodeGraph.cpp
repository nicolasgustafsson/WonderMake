#include "pch.h"
#include "NodeGraph.h"

NodeGraph::NodeGraph()
{
	static u64 uniqueId;
	UniqueId = uniqueId;

	uniqueId++;
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
