#pragma once
#include "NodeGraph/NodeGraph.h"

#include "wondermake-utility/FilePath.h"

class RenderTarget;

class RenderNodeGraph
	: public NodeGraph
{
public:
	RenderNodeGraph(FilePath aPath);

	RenderTarget* GetFinalRenderTarget() const;
protected:
	virtual void RegisterNodes() override;

	void PostLoad() override;
};
