#pragma once
#include "NodeGraph/NodeTypes.h"
#include <soloud.h>
#include <filesystem>

namespace NodeTypes
{
	struct SAudioMixingResultNode : public SNodeType<SAudioMixingResultNode>
	{
		SAudioMixingResultNode() : SNodeType("Final Mix")
		{
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Mix");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};

	struct SSoundEffectResultNode : public SNodeType<SSoundEffectResultNode>
	{
		SSoundEffectResultNode() : SNodeType("Play sound effect on bus")
		{
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Sound effect");
			AddSlot<std::string>(ESlotIo::Input, "Bus name");
			AddSlot<f32>(ESlotIo::Input, "Volume");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};

	struct SAudioMixNode : public SNodeType<SAudioMixNode>
	{
		SAudioMixNode() : SNodeType("Mix")
		{ 
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "First");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Second");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Mixed");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};

	struct SAudioSourceBusNode : public SNodeType<SAudioSourceBusNode>
	{
		SAudioSourceBusNode() : SNodeType("Source Bus")
		{
			AddSlot<std::string>(ESlotIo::Input, "Bus Name");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Audio Bus");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};

	struct SEchoFilter : public SNodeType<SEchoFilter>
	{
		SEchoFilter() : SNodeType("Echo Filter")
		{
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Input");
			AddSlot<f32>(ESlotIo::Input, "Delay", 0.5f);
			AddSlot<f32>(ESlotIo::Input, "Decay", 0.5f);
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Output");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};

	struct SGetSoundEffect : public SNodeType<SGetSoundEffect>
	{
		SGetSoundEffect() : SNodeType("Get sound effect")
		{
			AddSlot<std::filesystem::path>(ESlotIo::Input, "Sound effect name"); //[Nicos]: TODO make this a path
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Output");
		}

		virtual void ExecuteNode(SNode& aNode) override;
	};
}

namespace SlotColors
{
	template<>
	inline ImColor GetColor<SoLoud::AudioSource*>()
	{
		return ImColor(255, 192, 32, 255);
		//return ImColor(128, 192, 255, 255); // teal
	}
}