#include "pch.h"
#include "NodeGraph.h"

NodeGraph::NodeGraph()
{
	static u64 uniqueId;
	UniqueId = uniqueId;

	uniqueId++;
}
