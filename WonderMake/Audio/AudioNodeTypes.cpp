#include "pch.h"
#include "AudioNodeTypes.h"
#include "Audio/AudioManager.h"
#include <soloud_echofilter.h>

namespace NodeTypes
{
	void SAudioMixingResultNode::ExecuteNodeLeftToRight(SNode& aNode)
	{
		SoLoud::Bus* audioSource = aNode.GetInput<SoLoud::Bus*>(0);
		SoLoud::Soloud& soloudEngine = SystemPtr<AudioManager>()->GetSoloudEngine();
		
		soloudEngine.stopAll();

		if (audioSource)
			soloudEngine.play(*audioSource);
	}

	void SAudioMixNode::PrepareNode(SNode& aNode)
	{
		std::any& audioMix = aNode.NodeData["AudioMix"];

		SoLoud::Bus* busPointer = nullptr;

		if (audioMix.has_value())
		{
			busPointer = &std::any_cast<SoLoud::Bus&>(audioMix);
			aNode.SetOutput<SoLoud::Bus*>(0, busPointer);
		}
		else
		{
			busPointer = &audioMix.emplace<SoLoud::Bus>(SoLoud::Bus());
		}

		busPointer->setFilter(0, nullptr);
		busPointer->setFilter(1, nullptr);
		busPointer->setFilter(2, nullptr);
		busPointer->setFilter(3, nullptr);

		aNode.SetOutput<SoLoud::Bus*>(0, busPointer);
	}

	void SAudioMixNode::ExecuteNodeLeftToRight(SNode& aNode)
	{
		std::any& audioMix = aNode.NodeData["AudioMix"];
		SoLoud::Bus& busPointer = std::any_cast<SoLoud::Bus&>(audioMix);

		auto* firstSource = aNode.GetInput<SoLoud::AudioSource*>(0);
		auto* secondSource = aNode.GetInput<SoLoud::AudioSource*>(1);

		if (firstSource)
			busPointer.play(*firstSource);

		if (secondSource)
			busPointer.play(*secondSource);
	}

	void SAudioSourceBusNode::PrepareNode(SNode& aNode)
	{
		SystemPtr<AudioManager> audioManager;

		SoLoud::Bus& bus = audioManager->GetBus(aNode.GetInput<std::string>(0));

		bus.setFilter(0, nullptr);
		bus.setFilter(1, nullptr);
		bus.setFilter(2, nullptr);
		bus.setFilter(3, nullptr);

		aNode.SetOutput<SoLoud::Bus*>(0, &bus);
	}

	void SEchoFilter::PrepareNode(SNode& aNode)
	{
		auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);

		aNode.SetOutput(0, audioSource);
	}

	void SEchoFilter::ExecuteNodeLeftToRight(SNode& aNode)
	{
		auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);

		if (audioSource)
		{
			const f32 delay = aNode.GetInput<f32>(1);
			const f32 decay = aNode.GetInput<f32>(2);

			std::any& anyFilter = aNode.NodeData["filter"];

			SoLoud::EchoFilter* filter = nullptr;

			if (anyFilter.has_value())
			{
				filter = &std::any_cast<SoLoud::EchoFilter&>(anyFilter);
			}
			else
			{
				filter = &anyFilter.emplace<SoLoud::EchoFilter>();
			}

			filter->setParams(delay, decay);

			audioSource->setFilter(0, filter);
		}
	}

	void SSoundEffectResultNode::ExecuteNodeLeftToRight(SNode& aNode)
	{
		auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);

		if (!audioSource)
			return;

		const std::string busName = aNode.GetInput<std::string>(1);
		const f32 volume = aNode.GetInput<f32>(2);

		SystemPtr<AudioManager> audioManager;
		SoLoud::Bus& bus = audioManager->GetBus(busName);
		
		bus.play(*audioSource, volume);
	}

	void SGetSoundEffect::PrepareNode(SNode& aNode)
	{
		const std::filesystem::path audioEffectName = aNode.GetInput<std::filesystem::path>(0);

		ResourceProxy<AudioFile> audioFile = SystemPtr<ResourceSystem<AudioFile>>()->GetResource(audioEffectName);

		std::any& resourceSave = aNode.NodeData["resource"];

		resourceSave.emplace<ResourceProxy<AudioFile>>(audioFile);
		
		SoLoud::Wav& wav = audioFile->GetSource();

		wav.setFilter(0, nullptr);
		wav.setFilter(1, nullptr);
		wav.setFilter(2, nullptr);
		wav.setFilter(3, nullptr);
		//wav.setSingleInstance(true);

		aNode.SetOutput<SoLoud::AudioSource*>(0, &wav);
	}
}
