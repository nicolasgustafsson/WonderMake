#pragma once

#include "Message/Messages.h"
#include "Message/MessageSubscriber.h"

#include <fstream>

class FileLogger
{
public:
	FileLogger() noexcept;

private:
	void OnLogMessage(const SLogMessage& aMessage);
	MessageSubscriber mySubscriber;

	std::ofstream myFileStream;
};
