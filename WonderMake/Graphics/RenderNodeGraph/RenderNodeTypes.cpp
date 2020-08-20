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

	//
	//void SEchoFilter::PrepareNode(SNode& aNode)
	//{
	//	auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);
	//
	//	aNode.SetOutput(0, audioSource);
	//}
	//
	//void SEchoFilter::ExecuteNodeLeftToRight(SNode& aNode)
	//{
	//	auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);
	//
	//	if (audioSource)
	//	{
	//		const f32 delay = aNode.GetInput<f32>(1);
	//		const f32 decay = aNode.GetInput<f32>(2);
	//
	//		std::any& anyFilter = aNode.NodeData["filter"];
	//
	//		auto& filter = anyFilter.has_value() ? std::any_cast<SoLoud::EchoFilter&>(anyFilter) : anyFilter.emplace<SoLoud::EchoFilter>();
	//
	//		filter.setParams(delay, decay);
	//
	//		audioSource->setFilter(0, &filter);
	//	}
	//}
	//
	//void SSoundEffectResultNode::ExecuteNodeLeftToRight(SNode& aNode)
	//{
	//	auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);
	//
	//	if (!audioSource)
	//		return;
	//
	//	const std::string busName = aNode.GetInput<std::string>(1);
	//	const f32 volume = aNode.GetInput<f32>(2);
	//
	//	SystemPtr<AudioManager> audioManager;
	//	SoLoud::Bus& bus = audioManager->GetBus(busName);
	//	
	//	bus.play(*audioSource, volume);
	//}
	//
	//void SGetSoundEffect::PrepareNode(SNode& aNode)
	//{
	//	const std::filesystem::path audioEffectName = aNode.GetInput<std::filesystem::path>(0);
	//
	//	ResourceProxy<AudioFile> audioFile = SystemPtr<ResourceSystem<AudioFile>>()->GetResource(audioEffectName);
	//
	//	std::any& resourceSave = aNode.NodeData["resource"];
	//
	//	resourceSave.emplace<ResourceProxy<AudioFile>>(audioFile);
	//	
	//	SoLoud::Wav& wav = audioFile->GetSource();
	//
	//	wav.setFilter(0, nullptr);
	//	wav.setFilter(1, nullptr);
	//	wav.setFilter(2, nullptr);
	//	wav.setFilter(3, nullptr);
	//	//wav.setSingleInstance(true);
	//
	//	aNode.SetOutput<SoLoud::AudioSource*>(0, &wav);
	//}

	void SProcessRenderCommandQueue::ExecuteNodeLeftToRight(struct SNode& aNode)
	{
		auto renderTarget = aNode.GetInput<std::shared_ptr<RenderTarget>>(0);

		if (!renderTarget)
			return;

		renderTarget->BindAsTarget();

		SystemPtr<RenderCommandProcessor>()->ProcessQueue();
		aNode.SetOutput(0, renderTarget);
	}

}
