#pragma once
#include "NodeGraph/NodeGraph.h"

class RenderTarget;

class RenderNodeGraph
	: public NodeGraph
{
public:
	RenderNodeGraph(std::filesystem::path aPath);

	RenderTarget* GetFinalRenderTarget() const;
protected:
	virtual void RegisterNodes() override;

	void PostLoad() override;
};
