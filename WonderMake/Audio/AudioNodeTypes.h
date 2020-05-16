#pragma once
#include "NodeGraph/Node.h"
#include "AudioMixingNodeGraph.h"

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

	struct SAudioNodeGraphNode : public NodeType<SAudioNodeGraphNode>
	{
		SAudioNodeGraphNode() : NodeType("Audio Node Graph")
		{
			AddSlot<AudioMixingNodeGraph>(ESlotIo::Input, "Node Graph In");
			AddSlot<AudioMixingNodeGraph>(ESlotIo::Output, "Node Graph Out");
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

namespace InputSlotEdits
{
	template<>
	inline void EditInputSlot<AudioMixingNodeGraph>(AudioMixingNodeGraph& aInput)
	{
		if (ImGui::Button("Show Node Graph"))
			aInput.ShouldBeVisible = true;

		WmGui::NodeGraphEditor::NodeGraphEdit(aInput);
	}
}
