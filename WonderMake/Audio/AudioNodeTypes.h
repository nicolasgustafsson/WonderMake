#pragma once
#include "NodeGraph/Node.h"

namespace AudioFlow
{
	struct SAudioFlowDummy {};
}

namespace NodeTypes
{
	struct SAudioMixingResultNode : public NodeType<SAudioMixingResultNode>
	{
		SAudioMixingResultNode() : NodeType("Final Mix")
		{
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Input, "Mix");
		}
	};

	struct SAudioMixNode : public NodeType<SAudioMixNode>
	{
		SAudioMixNode() : NodeType("Mix")
		{
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Input, "First");
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Input, "Second");
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Output, "Mixed");
		}
	};

	struct SAudioFilterNode : public NodeType<SAudioFilterNode>
	{
		SAudioFilterNode() : NodeType("Filter")
		{
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Input, "Input");
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Output, "Filtered");
		}
	};

	struct SAudioSourceBusNode : public NodeType<SAudioSourceBusNode>
	{
		SAudioSourceBusNode() : NodeType("Source Bus")
		{
			AddSlot<std::string>(ESlotIo::Input, "Bus Name");
			AddSlot<AudioFlow::SAudioFlowDummy>(ESlotIo::Output, "Audio Wave");
		}
	};
}

namespace SlotColors
{
	template<>
	inline ImColor GetColor<AudioFlow::SAudioFlowDummy>()
	{
		return ImColor(255, 192, 32, 255);
		//return ImColor(128, 192, 255, 255); // teal
	}
}
