#include "pch.h"
#include "RenderNodeTypes.h"
#include "Graphics/RenderTarget.h"
#include <any>
#include "Graphics/RenderCommandProcessor.h"
#include "Debugging/DebugLineDrawer.h"
#include "Graphics/ScreenPassRenderObject.h"
#include "Graphics/RenderSettingsManager.h"
#include "NodeGraph/NodeGraph.h"

namespace NodeTypes
{
	void SRenderResultNode::ExecuteNode(SNode& aNode)
	{
		std::any& finalRenderTarget = aNode.NodeData["RenderTarget"];

		finalRenderTarget = std::make_any<std::shared_ptr<RenderTarget>>(aNode.GetInput<std::shared_ptr<RenderTarget>>(0));
	}

	void SRenderTextureNode::ExecuteNode(SNode& aNode)
	{
		std::any& renderTargetAny = aNode.NodeData["RenderTarget"];
		const f32 renderScale = aNode.GetInput<f32>(0);

		SVector2f viewportSize = std::any_cast<SVector2f>(aNode.NodeGraph->myGlobalData["ViewportSize"]);

		viewportSize *= renderScale;

		if (viewportSize.X <= 0 || viewportSize.Y <= 0)
			return;

		const SRenderTargetSettings settings{ SVector2u(std::lroundf(viewportSize.X), std::lroundf(viewportSize.Y)), aNode.GetInput<bool>(1) };

		if (!renderTargetAny.has_value())
			renderTargetAny = std::move(std::make_any<std::shared_ptr<RenderTarget>>(std::make_shared<RenderTarget>(settings)));

		std::shared_ptr<RenderTarget> renderTarget = std::any_cast<std::shared_ptr<RenderTarget>>(renderTargetAny);

		if (renderTarget->GetSettings() != settings)
			renderTarget->Reinitialize(settings);

		renderTarget->BindAsTarget();

		SystemPtr<OpenGLFacade> openGl;
		openGl->SetClearColor(aNode.GetInput<SColor>(2));
		openGl->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		aNode.SetOutput<std::shared_ptr<RenderTarget>>(0, renderTarget);
	}

	void SProcessRenderLayer::ExecuteNode(SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();
		std::string renderLayer = aNode.GetInput<std::string>(1);

		const auto renderSettings = aNode.GetInput<SRenderSettings>(2);
		SystemPtr<RenderSettingsManager>()->PushSettings(renderSettings);

		SystemPtr<RenderCommandProcessor>()->GetRenderLayer(renderLayer).ProcessQueue();

		SystemPtr<RenderSettingsManager>()->PopSettings();

		aNode.SetOutput(0, renderTarget);
	}

	void SRenderDebugLines::ExecuteNode(SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();

		SystemPtr<DebugLineDrawer>()->Render();

		aNode.SetOutput(0, renderTarget);
	}

	void SPostProcess::ExecuteNode(SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);
		aNode.SetOutput(0, renderTarget);

		auto filePath = aNode.GetInput<std::filesystem::path>(3);

		if (!std::filesystem::exists(filePath))
			return;

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();

		auto texture = aNode.GetInput<std::shared_ptr<RenderTarget>>(1);

		if (texture)
			texture->BindAsTexture();

		auto texture2 = aNode.GetInput<std::shared_ptr<RenderTarget>>(2);

		if (texture2)
			texture2->BindAsTexture(1);

		std::any& screenPassAny = aNode.NodeData["RenderTarget"];

		if (!screenPassAny.has_value())
		{
			SystemPtr<ResourceSystem<Shader<EShaderType::Vertex>>> vsSystem;
			SystemPtr<ResourceSystem<Shader<EShaderType::Fragment>>> fsSystem;
			SystemPtr<ResourceSystem<Shader<EShaderType::Geometry>>> gsSystem;

			screenPassAny = std::move(std::make_any<std::shared_ptr<ScreenPassRenderObject>>(std::make_shared<ScreenPassRenderObject>(*vsSystem, *fsSystem, *gsSystem, aNode.GetInput<std::filesystem::path>(3))));
		}

		std::shared_ptr<ScreenPassRenderObject> screenPass = std::any_cast<std::shared_ptr<ScreenPassRenderObject>>(screenPassAny);

		auto renderSettings = aNode.GetInput<SRenderSettings>(4);
		SystemPtr<RenderSettingsManager>()->PushSettings(renderSettings);

		screenPass->RenderImmediate();

		SystemPtr<RenderSettingsManager>()->PopSettings();
	}

	void SClearDepth::ExecuteNode(SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();
		float depth = aNode.GetInput<float>(1);

		SystemPtr<OpenGLFacade> openGl;
		openGl->SetClearDepth(depth);
		openGl->Clear(GL_DEPTH_BUFFER_BIT);
		openGl->SetClearDepth(0.f);

		aNode.SetOutput(0, renderTarget);
	}
}
