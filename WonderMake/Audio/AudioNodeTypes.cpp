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
		SoLoud::Bus* busPointer = nullptr;
		busPointer = &std::any_cast<SoLoud::Bus&>(audioMix);

		auto* firstSource = aNode.GetInput<SoLoud::AudioSource*>(0);
		auto* secondSource = aNode.GetInput<SoLoud::AudioSource*>(1);

		if (firstSource)
			busPointer->play(*firstSource);

		if (secondSource)
			busPointer->play(*secondSource);
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

			static SoLoud::EchoFilter filter;
			filter.setParams(delay, decay);
			audioSource->setFilter(0, &filter);

		}
	}
}
