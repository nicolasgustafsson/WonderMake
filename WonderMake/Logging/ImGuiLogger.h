#pragma once
#include "Message/MessageSubscriber.h"
#include <Utilities/Debugging/Debugged.h>

struct SImGuiLogMessage
{
	SColor Color;
	std::string Message;
};

class ImGuiLogger
	: public Debugged
{
public:
	ImGuiLogger();
	~ImGuiLogger() = default;

private:
	virtual void Debug() override;
	void OnLogMessage(const SLogMessage& aMessage);

	std::vector<SImGuiLogMessage> myLogMessages;

	std::string myFilterText;

	MessageSubscriber mySubscriber;

	bool myIsAtBottom = true;
	std::optional<f32> myPreviousScrollY;
};

