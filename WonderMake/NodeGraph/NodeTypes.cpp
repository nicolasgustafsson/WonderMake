#include "pch.h"
#include "NodeTypes.h"

SConnection::~SConnection()
{
	if (InputSlot && InputSlot->Connection == this)
		InputSlot->Connection = nullptr;

	if (OutputSlot)
	{
		auto& outputSlotConnections = OutputSlot->Connections;

		auto outputSlotConnectionIt = std::find(outputSlotConnections.begin(), outputSlotConnections.end(), this);
		if (outputSlotConnectionIt != outputSlotConnections.end())
			outputSlotConnections.erase(outputSlotConnectionIt);
	}
}

SConnection::SConnection(SConnection&& aOther)
{
	Color = aOther.Color;
	InputNodePointer = aOther.InputNodePointer;
	OutputNodePointer = aOther.OutputNodePointer;
	InputSlot = aOther.InputSlot;
	OutputSlot = aOther.OutputSlot;

	aOther.InputNodePointer = nullptr;
	aOther.OutputNodePointer = nullptr;
	aOther.InputSlot = nullptr;
	aOther.OutputSlot = nullptr;
}

SConnection& SConnection::operator=(SConnection&& aOther)
{
	Color = aOther.Color;
	InputNodePointer = aOther.InputNodePointer;
	OutputNodePointer = aOther.OutputNodePointer;
	InputSlot = aOther.InputSlot;
	OutputSlot = aOther.OutputSlot;

	aOther.InputNodePointer = nullptr;
	aOther.OutputNodePointer = nullptr;
	aOther.InputSlot = nullptr;
	aOther.OutputSlot = nullptr;

	return *this;
}

std::optional<i32> SNode::GetIndexOfInputSlot(SInputSlotInstanceBase* aSlot) const
{
	for (size_t i = 0; i < InputSlotInstances.size(); i++)
		if (InputSlotInstances[i].get() == aSlot)
			return static_cast<i32>(i);

	return {};
}

std::optional<i32> SNode::GetIndexOfOutputSlot(SOutputSlotInstanceBase* aSlot) const
{
	for (size_t i = 0; i < OutputSlotInstances.size(); i++)
		if (OutputSlotInstances[i].get() == aSlot)
			return static_cast<i32>(i);

	return {};
}

void SNode::ClearNodeData()
{
	NodeData.clear();
}
