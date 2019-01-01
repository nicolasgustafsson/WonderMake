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

	void Draw();
private:
	void OnLogMessage(const SLogMessage& aMessage);

	std::vector<SImGuiLogMessage> myLogMessages;

	MessageSubscriber mySubscriber;
};

