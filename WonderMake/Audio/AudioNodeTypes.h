#pragma once
#include "NodeGraph/NodeTypes.h"
#include <soloud.h>

namespace NodeTypes
{
	struct SAudioMixingResultNode : public SNodeType<SAudioMixingResultNode>
	{
		SAudioMixingResultNode() : SNodeType("Final Mix")
		{
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Mix");
		}

		virtual void ExecuteBackwards(SNode& aNode) override;
	};

	struct SAudioMixNode : public SNodeType<SAudioMixNode>
	{
		SAudioMixNode() : SNodeType("Mix")
		{ 
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "First");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Second");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Mixed");
		}

		virtual void Execute(SNode& aNode) override;
		virtual void ExecuteBackwards(SNode& aNode) override;
	};

	struct SAudioSourceBusNode : public SNodeType<SAudioSourceBusNode>
	{
		SAudioSourceBusNode() : SNodeType("Source Bus")
		{
			AddSlot<std::string>(ESlotIo::Input, "Bus Name");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Audio Bus");
		}

		virtual void Execute(SNode& aNode) override;
	};

	struct SEchoFilter : public SNodeType<SEchoFilter>
	{
		SEchoFilter() : SNodeType("Echo Filter")
		{
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Input, "Input");
			AddSlot<f32>(ESlotIo::Input, "Delay");
			AddSlot<f32>(ESlotIo::Input, "Decay");
			AddSlot<SoLoud::AudioSource*>(ESlotIo::Output, "Output");
		}

		virtual void Execute(SNode& aNode) override;

		virtual void ExecuteBackwards(SNode& aNode) override;
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