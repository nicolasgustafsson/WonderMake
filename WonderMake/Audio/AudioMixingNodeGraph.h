#pragma once
#include "NodeGraph/NodeGraph.h"

class AudioMixingNodeGraph : public NodeGraph
{
public:
	AudioMixingNodeGraph(std::filesystem::path aPath);

protected:
	virtual void RegisterNodes() override;

	void PostLoad() override;
};
