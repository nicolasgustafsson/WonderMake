#pragma once
#include "Message/MessageSubscriber.h"

struct SImGuiLogMessage
{
	SColor Color;
	std::string Message;
};

class ImGuiLogger
{
public:
	ImGuiLogger();
	~ImGuiLogger() = default;

private:
	void OnDebugMessage(const SDebugMessage&);
	void OnLogMessage(const SLogMessage& aMessage);

	std::vector<SImGuiLogMessage> myLogMessages;

	std::string myFilterText;

	MessageSubscriber mySubscriber;
	MessageSubscriber myDebugSubscriber;
};

