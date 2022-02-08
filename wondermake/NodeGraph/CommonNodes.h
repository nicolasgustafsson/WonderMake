#pragma once
#include "NodeTypes.h"

namespace NodeTypes
{
	struct SAddFloatsNode : public SNodeType<SAddFloatsNode>
	{
		SAddFloatsNode() : SNodeType("Add Floats")
		{
			AddSlot<float>(ESlotIo::Input, "First");
			AddSlot<float>(ESlotIo::Input, "Second");
			AddSlot<float>(ESlotIo::Output, "Result");
		}

		//[Nicos]: TODO Implement execution
	};

	struct SPickColorNode : public SNodeType<SPickColorNode>
	{
		SPickColorNode() : SNodeType("Pick Color")
		{
			AddSlot<SColor>(ESlotIo::Input, "Input Color");
			AddSlot<SColor>(ESlotIo::Output, "Output Color");
		}

		//[Nicos]: TODO Implement execution
	};
}
