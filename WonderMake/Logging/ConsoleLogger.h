#pragma once
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"

enum class EConsoleColor
{
	Red,
	Blue,
	Green,
	Yellow,
	Default
};

class ConsoleLogger
{
public:
	ConsoleLogger() noexcept;
	~ConsoleLogger() = default;

private:
	void OnLogMessage(const SLogMessage& aMessage);
	void SetColor(const EConsoleColor aColor) noexcept;
	MessageSubscriber mySubscriber;
};

