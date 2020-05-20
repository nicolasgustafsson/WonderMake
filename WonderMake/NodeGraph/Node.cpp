#include "pch.h"
#include "Node.h"

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
