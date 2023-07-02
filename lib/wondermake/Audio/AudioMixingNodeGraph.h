#pragma once
#include "NodeGraph/NodeGraph.h"

#include "wondermake-utility/FilePath.h"

class AudioMixingNodeGraph : public NodeGraph
{
public:
	AudioMixingNodeGraph(FilePath aPath);

protected:
	virtual void RegisterNodes() override;

	void PostLoad() override;
};
