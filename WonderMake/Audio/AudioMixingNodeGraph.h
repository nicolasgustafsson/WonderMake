#pragma once
#include "NodeGraph/NodeGraph.h"
#include <soloud.h>
#include "Audio/AudioNodeTypes.h"

struct SAudioNodeStack
{
	SNode& Node;
	std::function<void(SNode& aNode)>& ExecutionFunction;
};

class AudioMixingNodeGraph : public NodeGraph
{
public:
	AudioMixingNodeGraph(std::filesystem::path aPath);

	void Compile() override;

	virtual void Execute() override;

protected:
	virtual void RegisterNodes() override;

	void PostLoad() override;

private:

	std::vector<SoLoud::Bus> myBusses;

	std::vector<SCompiledNode> myCompiledNodeStack;

};
