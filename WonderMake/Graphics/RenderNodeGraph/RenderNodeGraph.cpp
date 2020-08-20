#include "pch.h"
#include "RenderNodeGraph.h"
#include "Graphics/RenderNodeGraph/RenderNodeTypes.h"

RenderNodeGraph::RenderNodeGraph(std::filesystem::path aPath)
	: NodeGraph(std::move(aPath))
{
}

RenderTarget* RenderNodeGraph::GetFinalRenderTarget() const
{
	if (!myRootNode)
		return nullptr;

	std::any& finalRenderTarget = myRootNode->NodeData["RenderTarget"];

	if (!finalRenderTarget.has_value())
		return nullptr;

	return std::any_cast<std::shared_ptr<RenderTarget>>(finalRenderTarget).get();
}

void RenderNodeGraph::RegisterNodes()
{
	RegisterRootNode<NodeTypes::SRenderResultNode>();
	RegisterNode<NodeTypes::SRenderTextureNode>();
	RegisterNode<NodeTypes::SProcessRenderCommandQueue>();
	//RegisterNode<NodeTypes::SAudioMixNode>();
	//RegisterNode<NodeTypes::SAudioSourceBusNode>();
	//RegisterNode<NodeTypes::SEchoFilter>();
}

void RenderNodeGraph::PostLoad()
{
	Compile();
}
