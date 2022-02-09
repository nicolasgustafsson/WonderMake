#include "pch.h"
#include "AudioNodeTypes.h"
#include "Audio/AudioManager.h"
#include <soloud_echofilter.h>
#include <soloud_bus.h>

namespace NodeTypes
{
	void SAudioMixingResultNode::ExecuteNode(SNode& aNode)
	{
		SystemPtr<AudioManager> audioManager;

		SoLoud::Bus* audioSource = aNode.GetInput<SoLoud::Bus*>(0);
		SoLoud::Soloud& soloudEngine = audioManager->GetSoloudEngine();
		
		soloudEngine.stopAll();

		if (audioSource)
		{
			auto handle = soloudEngine.play(*audioSource);
			soloudEngine.setRelativePlaySpeed(handle, 1.f);
		}
	}


	void SAudioMixNode::ExecuteNode(SNode& aNode)
	{
		std::any& audioMix = aNode.NodeData["AudioMix"];

		auto& bus = audioMix.has_value() ? std::any_cast<SoLoud::Bus&>(audioMix) : audioMix.emplace<SoLoud::Bus>();

		for (i32 i = 0; i < 8; i++)
		{
			bus.setFilter(i, nullptr);
		}

		aNode.SetOutput<SoLoud::Bus*>(0, &bus);

		SoLoud::Bus& busPointer = std::any_cast<SoLoud::Bus&>(audioMix);

		auto* firstSource = aNode.GetInput<SoLoud::AudioSource*>(0);
		auto* secondSource = aNode.GetInput<SoLoud::AudioSource*>(1);

		if (firstSource)
			busPointer.play(*firstSource);

		if (secondSource)
			busPointer.play(*secondSource);
	}

	void SAudioSourceBusNode::ExecuteNode(SNode& aNode)
	{
		SystemPtr<AudioManager> audioManager;

		SoLoud::Bus& bus = audioManager->GetBus(aNode.GetInput<std::string>(0));

		for (i32 i = 0; i < 8; i++)
		{
			bus.setFilter(i, nullptr);
		}

		aNode.SetOutput<SoLoud::Bus*>(0, &bus);
	}

	void SEchoFilter::ExecuteNode(SNode& aNode)
	{
		auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);

		aNode.SetOutput(0, audioSource);

		if (audioSource)
		{
			const f32 delay = aNode.GetInput<f32>(1);
			const f32 decay = aNode.GetInput<f32>(2);

			std::any& anyFilter = aNode.NodeData["filter"];

			auto& filter = anyFilter.has_value() ? std::any_cast<SoLoud::EchoFilter&>(anyFilter) : anyFilter.emplace<SoLoud::EchoFilter>();

			filter.setParams(delay, decay);

			for(i32 i = 0; i < 8; i++)
			{
				if (audioSource->mFilter[i] == nullptr)
					audioSource->setFilter(i, &filter);
			}
		}
	}


	void SSoundEffectResultNode::ExecuteNode(SNode& aNode)
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

	void SGetSoundEffect::ExecuteNode(SNode& aNode)
	{
		const std::filesystem::path audioEffectName = aNode.GetInput<std::filesystem::path>(0);

		SystemPtr<ResourceSystem<AudioFile>> audioFileResource;

		ResourceProxy<AudioFile> audioFile = audioFileResource->GetResource(audioEffectName);

		std::any& resourceSave = aNode.NodeData["resource"];

		resourceSave.emplace<ResourceProxy<AudioFile>>(audioFile);
		
		SoLoud::Wav& wav = audioFile->GetSource();

		for (i32 i = 0; i < 8; i++)
		{
			wav.setFilter(i, nullptr);
		}

		aNode.SetOutput<SoLoud::AudioSource*>(0, &wav);
	}
}
