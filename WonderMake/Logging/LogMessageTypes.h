#pragma once
#include "Message/MessageTypes.h"

struct SLogMessage : public Message<Dispatchable>
{
	std::string LogText;
};