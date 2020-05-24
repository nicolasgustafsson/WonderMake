#include "pch.h"
#include "AudioNodeTypes.h"
#include "Audio/AudioManager.h"
#include <soloud_echofilter.h>

namespace NodeTypes
{
	void SAudioMixingResultNode::ExecuteBackwards(SNode& aNode)
	{
		SoLoud::Bus* audioSource = aNode.GetInput<SoLoud::Bus*>(0);
		SoLoud::Soloud& soloudEngine = SystemPtr<AudioManager>()->GetSoloudEngine();
		
		soloudEngine.stopAll();

		if (audioSource)
			soloudEngine.play(*audioSource);
	}

	void SAudioMixNode::Execute(SNode& aNode)
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

		aNode.SetOutput<SoLoud::Bus*>(0, busPointer);
	}

	void SAudioMixNode::ExecuteBackwards(SNode& aNode)
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

	void SAudioSourceBusNode::Execute(SNode& aNode)
	{
		SystemPtr<AudioManager> audioManager;

		SoLoud::Bus& bus = audioManager->GetBus(aNode.GetInput<std::string>(0));

		aNode.SetOutput<SoLoud::Bus*>(0, &bus);
	}

	void SEchoFilter::Execute(SNode& aNode)
	{
		auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);

		aNode.SetOutput(0, audioSource);
	}

	void SEchoFilter::ExecuteBackwards(SNode& aNode)
	{
		auto* audioSource = aNode.GetInput<SoLoud::AudioSource*>(0);

		if (audioSource)
		{
			f32 delay = aNode.GetInput<f32>(1);
			f32 decay = aNode.GetInput<f32>(2);

			static SoLoud::EchoFilter filter;
			filter.setParams(delay, decay);
			audioSource->setFilter(0, &filter);

		}
	}
}
