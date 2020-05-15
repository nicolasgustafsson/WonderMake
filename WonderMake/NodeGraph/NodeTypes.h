#pragma once
#include "Node.h"

namespace NodeTypes
{
	struct SAddFloatsNode : public NodeType<SAddFloatsNode>
	{
		SAddFloatsNode() : NodeType("Add Floats")
		{
			AddSlot<float>(ESlotIo::Input, "First");
			AddSlot<float>(ESlotIo::Input, "Second");
			AddSlot<float>(ESlotIo::Output, "Result");
		}
	};

	struct SPickColorNode : public NodeType<SPickColorNode>
	{
		SPickColorNode() : NodeType("Pick Color")
		{
			AddSlot<SColor>(ESlotIo::Input, "Input Color");
			AddSlot<SColor>(ESlotIo::Output, "Output Color");
		}
	};
}
