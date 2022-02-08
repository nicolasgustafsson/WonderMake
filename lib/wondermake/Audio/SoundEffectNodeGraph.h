#pragma once
#include "NodeGraph/NodeGraph.h"

class SoundEffectNodeGraph : public NodeGraph
{
public:
	SoundEffectNodeGraph(std::filesystem::path aPath);

protected:
	virtual void RegisterNodes() override;

	void PostLoad() override;
};
