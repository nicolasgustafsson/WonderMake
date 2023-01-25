#include "RenderNodeGraph.h"
#include "Graphics/RenderNodeGraph/RenderNodeTypes.h"

_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<RenderNodeGraph>, RenderNodeGraphResource);

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
	RegisterNode<NodeTypes::SProcessRenderLayer>();
	RegisterNode<NodeTypes::SRenderDebugLines>();
	RegisterNode<NodeTypes::SPostProcess>();
	RegisterNode<NodeTypes::SClearDepth>();
}

void RenderNodeGraph::PostLoad()
{
	Compile();
}
