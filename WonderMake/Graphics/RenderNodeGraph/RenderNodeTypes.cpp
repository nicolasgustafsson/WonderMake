#include "pch.h"
#include "RenderNodeTypes.h"
#include "Graphics/RenderTarget.h"
#include <any>
#include "Graphics/RenderCommandProcessor.h"

namespace NodeTypes
{
	void SRenderResultNode::ExecuteNodeLeftToRight(SNode& aNode)
	{
		std::any& finalRenderTarget = aNode.NodeData["RenderTarget"];

		finalRenderTarget = std::make_any<std::shared_ptr<RenderTarget>>(aNode.GetInput<std::shared_ptr<RenderTarget>>(0));

	}

	void SRenderTextureNode::PrepareNode(SNode& aNode)
	{

	}

	void SRenderTextureNode::ExecuteNodeRightToLeft(struct SNode& aNode)
	{
		std::any& renderTargetAny = aNode.NodeData["RenderTarget"];
		SRenderTargetSettings settings{ aNode.GetInput<SVector2u>(0), aNode.GetInput<bool>(1) };

		if (!renderTargetAny.has_value())
			renderTargetAny = std::move(std::make_any<std::shared_ptr<RenderTarget>>(std::make_shared<RenderTarget>(settings)));

		std::shared_ptr<RenderTarget> renderTarget = std::any_cast<std::shared_ptr<RenderTarget>>(renderTargetAny);


		renderTarget->BindAsTarget();

		SystemPtr<OpenGLFacade> openGl;
		openGl->SetClearColor(SColor::RaisinBlack);
		openGl->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		aNode.SetOutput<std::shared_ptr<RenderTarget>>(0, renderTarget);
	}

	void SProcessRenderLayer::ExecuteNodeLeftToRight(struct SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();
		std::string renderLayer = aNode.GetInput<std::string>(1);

		SystemPtr<RenderCommandProcessor>()->GetRenderLayer(renderLayer).ProcessQueue();
		aNode.SetOutput(0, renderTarget);
	}

}
