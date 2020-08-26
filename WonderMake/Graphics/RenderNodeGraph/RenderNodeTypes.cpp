#include "pch.h"
#include "RenderNodeTypes.h"
#include "Graphics/RenderTarget.h"
#include <any>
#include "Graphics/RenderCommandProcessor.h"
#include "Debugging/DebugLineDrawer.h"
#include "Graphics/ScreenPassRenderObject.h"

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

	void SRenderDebugLines::ExecuteNodeLeftToRight(struct SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();

		SystemPtr<DebugLineDrawer>()->Render();

		aNode.SetOutput(0, renderTarget);
	}

	void SPostProcess::ExecuteNodeLeftToRight(struct SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);
		aNode.SetOutput(0, renderTarget);

		auto filePath = aNode.GetInput<std::filesystem::path>(2);

		if (!std::filesystem::exists(filePath))
			return;

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();

		auto texture = aNode.GetInput<std::shared_ptr<RenderTarget>>(1);

		if (!texture)
			return;

		texture->BindAsTexture();

		std::any& screenPassAny = aNode.NodeData["RenderTarget"];

		if (!screenPassAny.has_value())
			screenPassAny = std::move(std::make_any<std::shared_ptr<ScreenPassRenderObject>>(std::make_shared<ScreenPassRenderObject>(aNode.GetInput<std::filesystem::path>(2))));

		std::shared_ptr<ScreenPassRenderObject> screenPass = std::any_cast<std::shared_ptr<ScreenPassRenderObject>>(screenPassAny);

		screenPass->RenderImmediate();
	}
}
